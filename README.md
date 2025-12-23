# Agentive AI in C

This project is a minimal agent framework written in C. It demonstrates an "agentive" loop:
- accept a user goal,
- (optionally) call an LLM to produce a plan,
- dispatch plan steps to registered tools,
- return results.

Features
- Tools registry (add your own tools).
- Optional OpenAI-style LLM integration via HTTP (libcurl). If no API key is set, a local fallback planner is used.
- Example tools: `echo`, `calc` (simple arithmetic of `a + b`, `a - b`, `a * b`, `a / b`), `fetch` (HTTP GET).

Dependencies
- gcc (or clang)
- libcurl
- pkg-config
- cJSON (optional if you want JSON parsing later; not required for this minimal example)

Install on Debian/Ubuntu:
```
sudo apt update
sudo apt install build-essential libcurl4-openssl-dev pkg-config
```

Build
```
make
```

Run
```
# Provide a goal as an argument
./bin/agent "Summarize the headers of https://example.com"

# Or run interactively (reads the goal from stdin if no arg)
./bin/agent
Goal: Print Hello world using the echo tool
```

Environment variables
- OPENAI_API_KEY: optional. If set, the agent will attempt to call the Chat/Completion endpoint configured in code (edit endpoint if needed). If not set, a fallback local planner is used.

Security notes
- The provided `fetch` tool performs HTTP GET requests.
- This example does NOT execute arbitrary shells. Do not add an "exec" tool unless you sanitize inputs.
- Be careful sending sensitive data to external LLM services.

Extending
- Add tools by implementing a function of type `char *tool_fn(const char *input)` and registering it in `main`.
- Replace the simple LLM prompt/response parsing with robust JSON parsing (e.g., using `cJSON`) and a structured plan schema.
