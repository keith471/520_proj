

- how to treat string literals vs. raw string literals???
    - I can do var s string = "hello" + `raw` and this is fine. But does this give a raw string or an interpreted string?
    - ANSWER: every time we see a raw string, when we codegen it we iterate through it and add a backslash before any backslashes in the string.
- how can I check that every function program path has a return statement?
