
from flask import Flask, render_template, request, jsonify
import subprocess
import tempfile
import os
import re

app = Flask(__name__)

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
COMPILER = os.path.join(BASE_DIR, "conditional_compiler")


def find_heading(text, heading):
    pattern = (
        r"(?m)^[ \t]*"
        + re.escape(heading)
        + r"[ \t]*$"
    )
    return re.search(pattern, text, re.IGNORECASE)


def extract_section(text, start_heading, end_heading=None):
    start_match = find_heading(text, start_heading)

    if not start_match:
        return ""

    start = start_match.end()
    end = len(text)

    if end_heading:
        end_match = find_heading(text[start:], end_heading)

        if end_match:
            end = start + end_match.start()

    return text[start:end].strip()


def extract_syntax_result(text):
    match = re.search(
        r"Syntax analysis:\s*(SUCCESS|FAILED|FAILURE|ERROR)",
        text,
        re.IGNORECASE
    )

    if match:
        result = match.group(1).upper()

        if result == "FAILURE":
            result = "FAILED"

        return result

    if re.search(r"Syntax Error|Syntax analysis failed", text, re.IGNORECASE):
        return "FAILED"

    return "Not available"


def remove_ast_json(text):
    json_match = find_heading(text, "AST JSON")

    if not json_match:
        return text.strip()

    semantic_match = find_heading(text[json_match.end():], "Semantic Analysis")

    if semantic_match:
        semantic_start = json_match.end() + semantic_match.start()
        return (
            text[:json_match.start()].rstrip()
            + "\n\n"
            + text[semantic_start:].lstrip()
        ).strip()

    symbol_match = find_heading(text[json_match.end():], "Symbol Table")

    if symbol_match:
        symbol_start = json_match.end() + symbol_match.start()
        return (
            text[:json_match.start()].rstrip()
            + "\n\n"
            + text[symbol_start:].lstrip()
        ).strip()

    return text[:json_match.start()].strip()


@app.route("/")
def home():
    return render_template("index.html")


@app.route("/compile", methods=["POST"])
def compile_code():
    data = request.get_json(silent=True) or {}
    code = data.get("code", "")

    if not isinstance(code, str) or not code.strip():
        return jsonify({
            "error": "Please enter source code."
        }), 400

    if not os.path.isfile(COMPILER):
        return jsonify({
            "error": "Compiler not found. Run 'make' first."
        }), 500

    temp_path = None

    try:
        with tempfile.NamedTemporaryFile(
            mode="w",
            suffix=".src",
            encoding="utf-8",
            delete=False
        ) as source_file:
            source_file.write(code)
            temp_path = source_file.name

        result = subprocess.run(
            [COMPILER, temp_path],
            cwd=BASE_DIR,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            timeout=10,
            check=False
        )

        compiler_output = result.stdout or ""

        syntax = extract_syntax_result(compiler_output)

        ast = extract_section(
            compiler_output,
            "Abstract Syntax Tree (AST)",
            "AST JSON"
        )

        if not ast:
            ast = extract_section(
                compiler_output,
                "Abstract Syntax Tree (AST)",
                "Semantic Analysis"
            )

        semantic = extract_section(
            compiler_output,
            "Semantic Analysis",
            "Symbol Table"
        )

        clean_output = remove_ast_json(compiler_output)

        if syntax == "Not available" and result.returncode != 0:
            if "Semantic analysis: FAILED" in compiler_output:
                syntax = "SUCCESS"
            elif "Syntax Error" in compiler_output or "Syntax analysis: FAILED" in compiler_output:
                syntax = "FAILED"

        semantic_failed = (
            "Semantic analysis: FAILED" in compiler_output
            or "Semantic analysis failed:" in compiler_output
        )

        syntax_failed = (
            syntax == "FAILED"
            or "Syntax Error" in compiler_output
            or "Syntax analysis: FAILED" in compiler_output
        )

        compilation_success = (
            result.returncode == 0
            and not syntax_failed
            and not semantic_failed
        )

        if syntax == "Not available" and syntax_failed:
            syntax = "FAILED"

        if not semantic:
            if syntax_failed:
                semantic = "Semantic analysis was not run because syntax analysis failed."
            else:
                semantic = "No semantic output available."

        if not ast:
            ast = (
                "No AST available because syntax analysis failed."
                if syntax_failed
                else "No AST output available."
            )

        return jsonify({
            "success": compilation_success,
            "syntax": syntax,
            "ast": ast,
            "semantic": semantic,
            "output": clean_output or "No compiler output available."
        })

    except subprocess.TimeoutExpired:
        return jsonify({
            "error": "Compilation timed out after 10 seconds."
        }), 500

    except OSError as error:
        app.logger.exception("Unable to run the compiler")
        return jsonify({
            "error": "Unable to execute the compiler. Check its permissions and build."
        }), 500

    finally:
        if temp_path and os.path.exists(temp_path):
            os.remove(temp_path)


if __name__ == "__main__":
    app.run(host="127.0.0.1", port=9000, debug=True)
