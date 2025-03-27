% Opérateurs renommés
:-op( 800, fx, si ),
  op( 700, xfx, alors ),
  op( 300, xfy, ou ),
  op( 200, xfy, et ).
:- dynamic fait/1.

% Base de Faits
fait(a).


% Base de Connaissances
si a alors b.
si a et b alors c.

%Chainage arriere
%Verifie si le but est vrai
verifier(But):- ch_arriere(But).

% ch_arriere/1 : moteur d inference fonctionnant en chainage arriere
ch_arriere( But ) :- est_vrai( But ).
est_vrai( Proposition ) :- fait( Proposition ).
est_vrai( Proposition ) :- si Condition alors Proposition, est_vrai( Condition ).
est_vrai( Cond1 et Cond2 ) :- est_vrai( Cond1 ), est_vrai( Cond2 ).
est_vrai( Cond1 ou Cond2 ) :- est_vrai( Cond1 ) ; est_vrai( Cond2 ).

% Chainage avant
% Part des prémisses, des connaissances, donc, et cherche des nouveaux faits tant que possible.
deduire:- ch_avant.

% ch_avant/0 : moteur d inference en chainage avant
% à vous !


ch_avant :-
    si Condition alors Consequence, est_vrai(Condition), not(fait(Consequence)),!, assert(fait(Consequence)),
    
    ch_avant.