type tetCell =
  | I
  | J
  | L
  | O
  | S
  | T
  | Z;

type cell =
  | Full(tetCell)
  | Blank;

type action =
  | Left
  | Right
  | TurnLeft
  | TurnRight
  | Down;

type game;

let init: unit => game;

let act: (game, action) => game;

let isActive: game => bool;

let getBoard: game => list(list(cell));

let getLevel: game => int;

let getLines: game => int;

let getScore: game => int;
