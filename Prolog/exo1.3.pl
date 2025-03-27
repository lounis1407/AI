% Opérateurs renommés
:-op( 800, fx, si ),
  op( 700, xfx, alors ),
  op( 300, xfy, ou ),
  op( 200, xfy, et ).
:- dynamic fait/1.

% Déclaration du téléphone qui se trouve en G

telephone(g).


% Passages possibles
p(a, b).
p(b, c).
p(b, e).
p(c, d).
p(d, e).
p(e, f).
p(e, g).

% Si x appartient à la liste [X|Y], alors x appartient à la liste [Y]
appartient(X, [X|_]).

appartient(X, [_|Y]):-
  appartient(X, Y).


% Fonction de passage
aller(X, X, T).

aller(X, Y, T):-
  p(X, Z),
  not(appartient(Z, T)),
  aller(Z, Y, [X|T]).


aller(X, Y, T):-
  p(Z, X),
  not(appartient(Z, T)),
  aller(Z, Y, [X|T]).


% 1. aller(a, Y, []), telephone(Y).
% 2. telephone(Y), aller(a, Y, []).
% 3. telephone(Y), aller(a, Y, [d, f]).
