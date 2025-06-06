3-opt
---
There are 8 case for modifying 3 edges:

(a) original

    A -> [B -> C] -> [D -> E] -> F

(b) two-opt

    A -> [C -> B] -> [D -> E] -> F
reverse `[B -> C]` part

(c) two-opt

    A -> [B -> C] -> [E -> D] -> F
reverse `[E -> D]` part

(d) two-opt

    A -> [E -> D] -> [C -> B] -> F
reverse the whole `[E -> D -> C -> B]` part

(e) or-opt + two-opt

    A -> [D -> E] -> [C -> B] -> F
move `[B -> C]` to position after E and reverse it

(f) or-opt + two-opt

    A -> [E -> D] -> [B -> C] -> F
move `[D -> E]` to position before B and reverse it

(g) two-opt + two-opt

    A -> [C -> B] -> [E -> D] -> F
take 2-opt on both `[B -> C]` part and `[D -> E]` part

(h) or-opt

    A -> [D -> E] -> [B -> C] -> F
move `[D -> E]` to position before B
or
move `[B -> C]` to position after E 