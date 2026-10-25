
document.addEventListener("DOMContentLoaded", () => {
    const code = document.getElementById("code");
    const compileButton = document.getElementById("compile");
    const clearButton = document.getElementById("clear");

    const status = document.getElementById("status");
    const syntax = document.getElementById("syntax");
    const ast = document.getElementById("ast");
    const semantic = document.getElementById("semantic");
    const output = document.getElementById("output");

    const defaultMessages = {
        syntax: "Syntax result will appear here.",
        ast: "AST output will appear here.",
        semantic: "Semantic result will appear here.",
        output: "Compiler output will appear here."
    };

    function setStatus(message, type = "") {
        status.textContent = message;
        status.className = type;
    }

    function clearResults() {
        syntax.textContent = defaultMessages.syntax;
        ast.textContent = defaultMessages.ast;
        semantic.textContent = defaultMessages.semantic;
        output.textContent = defaultMessages.output;
    }

    function showFailure(message) {
        syntax.textContent = "Compilation failed.";
        ast.textContent = "No AST output available.";
        semantic.textContent = "No semantic output available.";
        output.textContent = message;
    }

    async function compileCode() {
        const source = code.value;

        if (!source.trim()) {
            setStatus("Please enter source code first.", "error");
            code.focus();
            return;
        }

        compileButton.disabled = true;
        clearButton.disabled = true;
        compileButton.textContent = "Compiling...";

        setStatus("Compiling your source code...", "loading");
        clearResults();

        try {
            const response = await fetch("/compile", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({
                    code: source
                })
            });

            let data;

            try {
                data = await response.json();
            } catch {
                throw new Error(
                    "The server returned an invalid response. Check app.py."
                );
            }

            if (!response.ok || data.error) {
                const message =
                    data.error || "Unable to compile your code.";

                showFailure(message);
                setStatus(message, "error");
                return;
            }

            syntax.textContent =
                data.syntax || "No syntax result available.";

            ast.textContent =
                data.ast || "No AST output available.";

            semantic.textContent =
                data.semantic || "No semantic output available.";

            output.textContent =
                data.output || "No compiler output available.";

            if (data.success) {
                setStatus(
                    "Compilation completed successfully.",
                    "success"
                );
            } else {
                setStatus(
                    "Compilation finished with errors. Check the results.",
                    "error"
                );
            }
        } catch (error) {
            const message =
                error.message ||
                "Could not connect to the Flask server.";

            setStatus(
                "Could not complete compilation. Check the Flask server.",
                "error"
            );

            showFailure(message);
        } finally {
            compileButton.disabled = false;
            clearButton.disabled = false;
            compileButton.textContent = "▶ Compile Code";
        }
    }

    function clearCode() {
        code.value = "";
        clearResults();
        setStatus("Editor cleared.");
        code.focus();
    }

    compileButton.addEventListener("click", compileCode);
    clearButton.addEventListener("click", clearCode);

    code.addEventListener("keydown", (event) => {
        if (event.ctrlKey && event.key === "Enter") {
            event.preventDefault();

            if (!compileButton.disabled) {
                compileCode();
            }
        }
    });
});
