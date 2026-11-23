# C++ Bank Management System

A console-based banking application written in C++ that combines client account management, user authentication, bitwise role permissions, and flat-file persistence (`Client.txt` and `Users.txt`).

## Why I Built This

I started with a basic client and transaction manager and then wanted to answer a more realistic systems question: *how do you protect actions when multiple staff users share the same application?* Instead of hardcoding roles or scattering `if/else` checks everywhere, I wired login state (`CurrentUser`) into a bitwise permission model (`enPermissions`) and backed both clients and users with simple delimited text records (`#//#`).

## Key Capabilities

- **Authentication & Session Flow**: Starts at `LoginScreen()`, verifies credentials against `Users.txt`, tracks the active session in `CurrentUser`, and supports logging out back to the login prompt.
- **Bitwise Permissions (`enPermissions`)**: Assigns granular access flags (`1`, `2`, `4`, `8`, `16`, `32`, `64`, or `-1` for full access) and checks rights with bitwise `&` before opening screens or menus.
- **Client Lifecycle & Transactions**: Supports listing clients, adding new accounts with duplicate-ID checks, updating details, soft-marking records for deletion, searching by account number, depositing, withdrawing with balance validation, and viewing total balances.
- **User Administration**: Allows authorized admins (`eManage_Users`) to list users, create accounts, update passwords/permissions, search users, and delete other users while preventing self-deletion.
- **File Persistence**: Serializes and deserializes records using `#//#` separators so client balances and user permissions survive across runs.

## Project Structure & Data Files

- `BankManagementSystem.cpp` — Full console application containing data structures, file helpers, screen handlers, permission checks, and menu loops.
- `Client.txt` — Created/used at runtime to store serialized client records (`Account_Number`, `PIN_Code`, `Client_Name`, `Client_Phone`, `Account_Balance`).
- `Users.txt` — Created/used at runtime to store serialized user records (`UserName`, `User_Password`, `Permissions`).

## Build & Run

Make sure `<limits>` is included at the top of `BankManagementSystem.cpp` alongside the standard headers, then compile with a C++ compiler:

```bash
g++ -std=c++17 BankManagementSystem.cpp -o BankManagementSystem
./BankManagementSystem
```

(Note: The application currently uses Windows console helpers such as system("cls") and system("pause>0") in its screen navigation flow.)

## Honest Limitations & Next Steps

- **Plain-Text Storage**: Passwords and PINs are stored in plain text inside `.txt` files—fine for learning file I/O and state flow, not for production security.
- **Input & Edge Cases**: Numeric prompts and screen re-entry loops can be tightened further against invalid non-numeric input.
- **Future Direction**: Split the single-file implementation into modular headers/modules and connect a richer storage layer or unit tests.
