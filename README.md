# 🚫 pg_block_if_slow — Block Costly Queries in PostgreSQL

Have you ever thought to yourself,  
_"Self, I wish I could stop expensive queries before they bring my database to its knees!"_

Well today is your lucky day, because now you absolutely can!

When enabled, the `pg_block_if_slow` extension prevents the execution of any query whose **estimated cost exceeds a defined threshold** — even before it runs!

Perfect for protecting your production system from:
- Unoptimized queries  
- Accidental full table scans  
- That one developer who loves `SELECT *` without a `WHERE`

---

## 🚀 Installation

Installing this extension is straightforward:

```bash
git clone [https://github.com/yourname/pg_block_if_slow.git](https://github.com/ESHWAR-333/pg_block_if_slow)
cd pg_block_if_slow
make
sudo make install
```
Then add pg_block_if_slow to shared_preload_libraries in your postgresql.conf, and you’re good to go:

```conf
shared_preload_libraries = 'pg_block_if_slow'
pg_block_if_slow.threshold = 100000
```
Restart PostgreSQL to load the extension.

You can optionally install the extension in SQL as well:
```
CREATE EXTENSION pg_block_if_slow;
```


## ⚙️ Configuration
This extension exposes a single configuration parameter:

pg_block_if_slow.threshold
- Type: ``real``

- Default: ``100000``

- Scope: ``USERSET`` (can be changed at session level)

- Description: The maximum estimated query cost allowed before a query is blocked.

You can set this value using any of the following:
```
SET pg_block_if_slow.threshold = 50000; -- session level

ALTER SYSTEM SET pg_block_if_slow.threshold = 75000; -- persistent
SELECT pg_reload_conf();
```


## 💡 Example

```
SET pg_block_if_slow.threshold = 1000;

-- This will throw an error if it's too expensive
EXPLAIN SELECT * FROM large_table WHERE non_indexed_column = 42;

```
Error:
```
ERROR: Query blocked: estimated cost 13450.50 exceeds threshold 1000.00
 
```

## 🧪 What’s Happening Internally?
This extension uses PostgreSQL’s internal planner hook to examine the query plan before it executes.

It does the following:

- Reads planTree->total_cost from the planned query

- Compares it with the configured threshold

- Throws an error if it exceeds the budget

✅ Result: No slow query ever reaches the executor.

## 📘 Use Cases
- Protect production from poorly written queries

- Enforce performance budgets in shared environments

- Catch missing indexes and accidental full scans early

- Educate developers via useful error messages

## ⚠️ Limitations
- Uses estimated planner cost, not actual runtime.

- Does not analyze subqueries individually — total cost only.

- Does not work on already-prepared queries (unless re-planned).

- Will block even read-only queries if their plan cost is too high.

## 🛠 Developer Notes
This extension uses:

- planner_hook to inject our logic

- total_cost from the PlannedStmt

- A custom GUC (pg_block_if_slow.threshold)

You can find similar examples in PostgreSQL’s source code under src/backend/optimizer/.

