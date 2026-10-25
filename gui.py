
import tkinter as tk
from tkinter import scrolledtext, messagebox
import subprocess
import tempfile
import os

COMPILER = os.path.join(os.path.dirname(__file__), "conditional_compiler")


def compile_code():
    code = editor.get("1.0", "end-1c")

    if not code.strip():
        messagebox.showwarning("Warning", "Enter source code first!")
        return

    temp = None

    try:
        with tempfile.NamedTemporaryFile(
            mode="w", suffix=".src", delete=False
        ) as file:
            file.write(code)
            temp = file.name

        result = subprocess.run(
            [COMPILER, temp],
            capture_output=True,
            text=True,
            timeout=10
        )

        output.delete("1.0", tk.END)
        output.insert(tk.END, result.stdout + result.stderr)

        status.config(
            text="Compilation Successful" if result.returncode == 0
            else "Compilation Failed",
            fg="green" if result.returncode == 0 else "red"
        )

    except Exception as error:
        messagebox.showerror("Error", str(error))

    finally:
        if temp and os.path.exists(temp):
            os.remove(temp)


def clear():
    editor.delete("1.0", tk.END)
    output.delete("1.0", tk.END)
    status.config(text="Ready", fg="black")


root = tk.Tk()
root.title("Conditional Statement Compiler")
root.geometry("800x600")
root.configure(bg="#eef2f7")

tk.Label(
    root,
    text="Conditional Statement Compiler",
    font=("Arial", 18, "bold"),
    bg="#172554",
    fg="white",
    pady=15
).pack(fill="x")

tk.Label(
    root, text="Source Code",
    font=("Arial", 12, "bold"),
    bg="#eef2f7"
).pack(anchor="w", padx=15, pady=5)

editor = scrolledtext.ScrolledText(
    root, height=12, font=("Courier New", 11)
)
editor.pack(fill="both", expand=True, padx=15, pady=5)

editor.insert(
    "1.0",
    "x = 10;\n"
    "if (x > 5) {\n"
    "    y = 20;\n"
    "} else {\n"
    "    y = 1;\n"
    "}\n"
)

buttons = tk.Frame(root, bg="#eef2f7")
buttons.pack(pady=8)

tk.Button(
    buttons, text="Compile",
    command=compile_code,
    bg="#2563eb", fg="white",
    padx=20, pady=5
).pack(side="left", padx=5)

tk.Button(
    buttons, text="Clear",
    command=clear,
    padx=20, pady=5
).pack(side="left", padx=5)

status = tk.Label(
    root, text="Ready",
    font=("Arial", 10, "bold"),
    bg="#eef2f7"
)
status.pack(pady=5)

tk.Label(
    root, text="Compiler Output",
    font=("Arial", 12, "bold"),
    bg="#eef2f7"
).pack(anchor="w", padx=15, pady=5)

output = scrolledtext.ScrolledText(
    root, height=10, font=("Courier New", 10)
)
output.pack(fill="both", expand=True, padx=15, pady=(0, 15))

root.mainloop()
