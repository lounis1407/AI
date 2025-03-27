% Opérateurs renommés
:- op(800, fx, si).
:- op(700, xfx, alors).
:- op(300, xfy, ou).
:- op(200, xfy, et).

:- dynamic fait/1.

% Base de Faits
fait(rhizome).
fait(fleur).
fait(graine).
fait(une_cotyledone).  % Remplace "1-cotyledone"

% Base de Connaissances
si fleur et graine alors phanerogame.
si phanerogame et graine_nue alors sapin.
si phanerogame et une_cotyledone alors monocotyledone.
si phanerogame et deux_cotyledones alors dicotyledone.
si monocotyledone et rhizome alors muquet.
si dicotyledone alors anemone.
si monocotyledone et non(rhizome) alors lilas.
si feuille et fleur alors cryptogame.
si cryptogame et non(racine) alors mousse.
si cryptogame et racine alors fougere.
si non(feuille) et plante alors thallophyte.
si thallophyte et chlorophylle alors algue.
si thallophyte et non(chlorophylle) alors champignon.
si non(feuille) et non(fleur) et non(plante) alors colibacille.

% Vérification des faits négatifs
non(X) :- \+ fait(X).

% Chainage arrière  
% Vérifie si un but est vrai
verifier(But) :- ch_arriere(But).

% ch_arriere/1 : moteur d’inférence en chaînage arrière
ch_arriere(But) :- est_vrai(But).
est_vrai(Proposition) :- fait(Proposition).
est_vrai(Proposition) :- si Condition alors Proposition, est_vrai(Condition).
est_vrai(Cond1 et Cond2) :- est_vrai(Cond1), est_vrai(Cond2).
est_vrai(Cond1 ou Cond2) :- est_vrai(Cond1) ; est_vrai(Cond2).

% Chainage avant
% Part des prémisses et cherche les nouveaux faits tant que possible.
deduire :- ch_avant.

% ch_avant/0 : moteur d’inférence en chaînage avant
ch_avant :-
    si (Cond) alors (Conseq),
    not(fait(Conseq)),
    est_vrai(Cond),
    !,
    assert(fait(Conseq)), 
    write("Nouveau fait : "), write(Conseq), nl,
    ch_avant.

ch_avant :- write("Fini"), nl.
