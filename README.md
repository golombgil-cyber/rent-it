# Rent It

A small, concurrent equipment-lending system built with C++ and TCP sockets on Linux.

This academic project was co-developed by Gil Zion Golomb and a project partner. It demonstrates client-server communication, shared-state coordination and a text-based protocol. The partner is intentionally unnamed. It is an educational prototype, not a production service or a secure authentication system.

## What it does

- Lists 15 predefined equipment items and their current availability.
- Supports borrowing and returning items under a client-supplied username.
- Rejects returns when that username does not match the recorded borrower.
- Lets a client wait until an item becomes available.
- Handles concurrent connections with a dedicated server thread per client.

## Architecture

```text
Terminal client(s) -- newline-delimited TCP --> Server / client threads
                                                   |
                                            InventoryManager
                                            mutex + condition variable
                                                   |
                                             In-memory Items
```

The client uses a separate listener thread to display server responses. `InventoryManager` serializes access to the shared inventory with a mutex. `WAIT` uses a condition variable and releases that mutex while waiting. Returning an item wakes waiting threads. The item is not reserved by `WAIT`: another client may borrow it before the waiting client sends `BORROW`.

## Build

Requirements: Linux, a C++17-capable compiler, POSIX sockets and pthread support. The optional smoke checks use Python 3's standard library.

```bash
make
```

This creates `build/server` and `build/client`.

## Run locally

**Use an isolated local development environment. The unchanged server binds to all IPv4 interfaces, not just localhost. Do not expose it to the Internet or an untrusted network.** It has no TLS, passwords, rate limits or persistent storage.

Terminal 1:

```bash
./build/server 5050
```

Terminal 2:

```bash
./build/client 127.0.0.1 5050
```

Type one command per line. Commands are uppercase; usernames use alphanumeric characters.

```text
HELLO alice
LIST
BORROW 1
RETURN 1
QUIT
```

For a two-client waiting example:

1. Client A sends `HELLO alice`, then `BORROW 1`.
2. Client B sends `HELLO bob`, then `WAIT 1`.
3. Client A sends `RETURN 1`.
4. Client B receives `OK AVAILABLE 1` and can then try `BORROW 1`.

While `WAIT` is blocked, that client's server thread does not process its later commands. Stop the server with Ctrl+C when done. Inventory is reset each time the server starts.

## Protocol overview

| Command | Behavior / example response |
| --- | --- |
| `HELLO <username>` | Sets a session label; `OK HELLO alice`. Not verified identity. |
| `LIST` | `LIST 15`, followed by one line per item. |
| `BORROW <id>` | `OK BORROWED 1`, or an error if unavailable. |
| `RETURN <id>` | `OK RETURNED 1`, or `ERR PERMISSION not_owner`. |
| `WAIT <id>` | Waits for availability; `OK AVAILABLE 1`. Does not borrow or reserve. |
| `QUIT` | `OK BYE`; closes the connection. |

Commands other than `HELLO` and `QUIT` require a username first. The code calls that state "authenticated", but there is no actual authentication: clients can choose or change the label freely. It must not be presented as an access-control security guarantee.

## Checks performed

The unchanged sources were compiled using `g++ -std=c++17 -Wall -Wextra -Wpedantic -pthread` on Linux with no compiler diagnostics in the checked run. A bounded local server-protocol smoke run passed 17 checks covering the initial inventory, two clients, competing borrowing, wrong-owner returns, waiting/notification, malformed IDs and clean protocol exits.

```bash
python3 docs/smoke_test.py
```

See `docs/smoke-test-results.json`. These are newly added preparation checks, not original coursework tests. They do not establish stress-test reliability, portability or security. The interactive client's terminal behavior has not been manually verified in this preparation.

## Known limitations

- Plaintext TCP and client-selected usernames; no verified identity or authorization boundary.
- Fixed in-memory inventory, no database or restart persistence.
- One detached thread per client, with no connection cap, line-length limit or read timeout.
- A disconnected client does not automatically release borrowed equipment.
- `WAIT` has no timeout or cancellation, no fairness guarantee and no atomic reservation.
- A waiting server thread may remain blocked after its client disconnects.
- Socket sends do not retry partial writes; peer-disconnect errors and `SIGPIPE` need stronger handling.
- Port conversion can throw on non-numeric input, and the command parser accepts some extra/trailing tokens.
- Shutdown, thread lifecycle and client EOF behavior need further work.

These are future engineering tasks, not fixes included in this package. See `docs/REVIEW.md`.

## Source and preparation

The six C++ source/header files in `src/` preserve the supplied coursework implementation. GitHub publication normalized final newlines in four files; no code logic was changed. `docs/source-manifest.json` records the original and published SHA-256 hashes. This preparation adds the build file, documentation, ignore rules and a smoke-check script/results. The original student submission file and its personal identifiers are excluded.

No open-source license has been chosen. No grant of reuse rights should be inferred.
