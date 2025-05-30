# ```task_graph_cpp_20250522/docs/previous_designs/design_dag_check.md```

This is an earlier design documents. Some details may not be up to date.

# Designing the DAG check for the composable task graph system

---

**WORK IN PROGRESS** - some information may be incorrect, and the design is subject to change.

---

## Choice of algorithm - Kahn's topological sorting

***NOTE*** This information may be incorrect. What we're checking is called "reachability", not "topological sorting", although the latter is a necessary side product of the reachability algorithm.

The algorithm used is equivalent to what is known as Kahn's algorithm for topological sorting.

Colloquially, we will just use "visit" (as both a noun and a verb) to refer to the algorithm's steps.

Various validation hooks are added to the algorithm to provide actionable feedback on failure.

---

## Types of nodes

1. General classification
    - Data nodes
    - Data connector nodes
    - Task nodes
    - Barrier nodes
    - "Other" nodes
1. Task nodes
    - Each task node is uniquely associated with a task waiting to be scheduled.
    - One or more inflows from any node type.
    - One or more outflows to any node type.
1. Plugin nodes
    - A plugin node is similar to a task node in that:
        - Eventually, it will be associated with a task waiting to be scheduled.
        - It can have one or more inflows from any node type, and outflows to any node type.
    - A plugin node does not require the executable code to be associated at the time of node creation.
    - This allowes the user to specify the code at a later time, as long as the code conforms to all expectations (inputs and outputs) of the plugin node.
    - Limitation: a plugin node does not allow the substitution of a task with a subgraph, i.e. something involving more than one task node.
        - Two ways to get around this limitation: a subordinate task graph executor can be instantiated to execute that subgraph to completion; or: use data nodes to allow the insertion of a subgraph.
1. Data nodes
    - Each data node is uniquely associated with a piece of data managed by a DataContext.
    - Each data node can only have one inflow.
        - If it is a global input node, its  inflow must come from the global start node.
        - Otherwise, its inflow must come from a task.
1. Data receptacle nodes
    - A data receptacle node is a placeholder node for data.
    - A data receptacle node accepts one inflow from a data node.
    - A data receptacle node can have outflows similar to the outflows of a data node.
    - This facilitates graph composition and editing.
    - During task graph execution, data receptable nodes are collapsed onto the real data node.
    - (Note) There is no counterpart (i.e. no data "outlet" node), because any data node can be "tapped" (read) as long as their node name is known.
1. Barrier nodes
    - Barrier nodes are no-op nodes that serve the sole purpose of blocking (i.e. being a barrier)
    - Barrier nodes are not associated with any task or data.
    - Barrier nodes are used to regulate the "dynamic width" of the BFS graph expansion
        - Prevents too many tasks from being added to the ready queue when it's already too long
        - Reduces memory build-up of temporaries due to processing rate imbalance in a pipeline or graph bottleneck
    - Barrier nodes cannot be essential.
        - Suppose the reachability algorithm ignores all barrier nodes, as if these nodes and their inflows / outflows were never present, that must not affect the algorithm outcome, meaning that the set of reachable nodes must be the same regardless of barriers.
        - If that is not the case, it is reported as a failure.
    - During task graph execution, barrier nodes between tasks are replaced with a direct link.
1. Specially designated nodes
    - Special barrier nodes
        - Global start node (unique)
        - Global stop node (unique)
    - Special data nodes
        - Global input nodes (one or more)
        - Global output nodes (one or more)

---

## Designated global start and global stop nodes

Global start node: label 0
Global stop node: label 1

All other node labels are assigned as needed; none are reserved.

## Global inputs and their connection to the global start node

Global inputs are data nodes that are already populated with data (i.e. done) before the task graph starts.

Each global input node has one incoming edge coming from the global start node.

As far as the global start node is concerned, these are the only allowed edges, plus an edge from the global start to the global stop.

## Global outputs and their connection to the global stop node

Global outputs are the data nodes that must be fully populated at the end of task graph execution.

Each global output has one outgoing edge leading to the global stop node.

These are the only data that will remain available after the task graph shutdown. All other data nodes are subject to expiration and/or pruning.

---

## First visit pass - the first run of Kahn's algorithm on the DAG

***NOTE*** Information might be wrong - not Kahn's algorithm. See top note about reachability.

We enumerate all nodes that are reachable from the global start node.

Some cycle types (but not all) might be detected during reachability; this will be discussed in the next section.

When that is done, the first thing we check is that the global stop node has been reached.

## Deviations from a fully-connected DAG

### Unreachable nodes, subgraphs, and cycles

Due to being completely isolated, these cycles and subgraph nodes are not reachable from the global start node, and hence never visited. They will be summary-reported after the first visit.

With additional metadata about nodes (e.g. if a subgraph builder was responsible for these never-visited nodes), the summary-report will highlight that as an actionable feedback.

### Global stop node made unreachable due to a never-met dependency from an unreachable node

#### Case 1 - some global outputs were unreachable

#### Case 2 - none of the global outputs were reached

## Pruning

Pruning is automatic.

During the 