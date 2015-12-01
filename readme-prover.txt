Prover - Propositional Resolution Theorem Prover

Version 1.1, 10-16-2015

Written by Larry Holder, Washington State University.

Prover is a propositional logic theorem prover using resolution and following
the design in Russell and Norvig's AI textbook. See sample program in test.cc
and sample KB in samplekb.txt.

Usage
-----

#include "Prover.h"
KnowledgeBase KB;
Sentence s(...); // ... is a string (see Sentence syntax below)

KB.Ask(s) - Return true or false as to whether sentence s is entailed by
            knowledge base; uses closed-world assumption. Outputs each
            resolution step.

KB.Tell(s) - Add sentence s to knowledge base.

KB.Remove(s) - Removes sentence s from KB. Specifically, removes all clauses
               generated from converting sentence s to clause form.

KB.Read(fileName) - Read sentences from file fileName and add to KB. File
                    has one sentence per line. Lines beginning with '%' are
                    comments. Blank lines allowed.

KB.Print() - Print knowledge base as list of clauses (CNF).

KB.Clear() - Clear knowledge base.


Propositional Logic Syntax
--------------------------

Sentence -> AtomicSentence | ComplexSentence
AtomicSentence -> Foo | Bar | Foo_Bar | ...	"string without below symbols"
ComplexSentence -> ( Sentence )
		| ~ Sentence			"not"
		| Sentence & Sentence		"and"
		| Sentence | Sentence		"or"
		| Sentence => Sentence		"implies"
		| Sentence <=> Sentence		"if and only if"

Operator Precedence: ~, &, |, =>, <=>


History
-------

Version 1.0, 09-30-2015
- Initial release

Version 1.1, 10-16-2015
- Simplified clauses to remove duplicate literals
- Removed redundant clauses (e.g., ((A v B) & (A v B v C)) = (A v B))
- Removed clauses with tautologies, i.e., contains A and ~A
 

