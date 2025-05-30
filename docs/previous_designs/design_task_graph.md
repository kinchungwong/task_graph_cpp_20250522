# ```task_graph_cpp_20250522/docs/previous_designs/design_task_graph.md```

This is an earlier design documents. Some details may not be up to date.

# Designing the task graph system

## In designing the task graph system, what does sweet success look like?

A sweet success would look like a breadboard (as in hobbyist electronics).

Each algorithm, itself a compute subgraph, is instantiated like an IC chip or a discrete element. Initially, it is plugged in somewhere on the breadboard - the requirement is that, initially, it shares **none** of the rows with any other placed elements.

Then, wires are added to connect the rows.

When the system is fully configured, a validator is run.

Once the system is validated, it can be connected to the power supply, and all parts of the system become active, to produce the result.

## Keeping in mind that all analogies are leaky, we should be mindful of differences between a task graph system and an electronics system. Explain these differences.

All parts of an electronics system are simultaneously active as long as power is supplied.

Meanwhile, each unique instance of task is executed exactly one time, after being triggered.

A task is triggered when all of its preconditions are met.

A precondition might be a piece of data (to be used as input), or another task.

## What are the pitfalls?

During the various phases of subgraph construction, there is no DAG.

Some dangling connections or disconnected islands are intentional. For example, it might be the user's intention that some outputs are not needed, and therefore their computations should be pruned.

But we cannot easily tell (guess) what the user's intentions are - we see that it's not a valid and connected DAG, and the user didn't say why.

## Addressing the gap between validation failures and user intentions (which it needs to clarify or expose)

The current thinking is that each algorithm subgraph provider must also provide a set of validation rules to cover their own use cases.

The validation rules are not required to be exhaustive; just enough to bridge this gap between validation failures and user intentions.

In particular, some rules require that some nodes have certain edges; other rules may say that some edges are not allowed.

## Sane defaults and user overrides

The default is not to allow silent failures. That means a particular deviation away from DAG will be rejected instead of being silently fixed. The user will still be able to express their intent by explicit means, such as discarding some results, omitting some steps, or editing the preset subgraph of an algorithm.
