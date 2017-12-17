let wellRows = 20;

let wellCols = 10;

type tetroid =
  | I
  | J
  | L
  | O
  | S
  | T
  | Z;

type cell =
  | Full(tetroid)
  | Blank;

let i = [[Full(I), Full(I), Full(I), Full(I)]];

let j = [[Full(J), Blank, Blank], [Full(J), Full(J), Full(J)]];

let l = [[Blank, Blank, Full(L)], [Full(L), Full(L), Full(L)]];

let o = [[Full(O), Full(O)], [Full(O), Full(O)]];

let s = [[Blank, Full(S), Full(S)], [Full(S), Full(S), Blank]];

let t = [[Full(T), Full(T), Full(T)], [Blank, Full(T), Blank]];

let z = [[Full(Z), Full(Z), Blank], [Blank, Full(Z), Full(Z)]];

type well = list(list(cell));

type action =
  | Left
  | Right
  | TurnRight
  | Down;

type activeGame = {
  well,
  score: int,
  gamePiece: list(list(cell)),
  gpRow: int,
  gpCol: int
};

type overGame = {
  well,
  score: int
};

type game =
  | Active(activeGame)
  | Over(overGame);

let init = () =>
  Active({
    well: Array.make_matrix(wellRows, wellCols, Blank) |> Array.to_list |> List.map(Array.to_list),
    score: 0,
    gamePiece: t,
    gpRow: 3,
    gpCol: 4
  });

let getActBoard = (a) => {
  let rec processCells = (pCells, wCells, colNum) =>
    switch (pCells, wCells, colNum < a.gpCol) {
    | (_, [], _) => [] /* This shouldn't happen */
    | ([], w, _) => w
    | (p, [w, ...ws], true) => [w, ...processCells(p, ws, colNum + 1)]
    | ([Blank, ...ps], [w, ...ws], false) => [w, ...processCells(ps, ws, colNum + 1)]
    | ([p, ...ps], [_, ...ws], false) => [p, ...processCells(ps, ws, colNum + 1)]
    };
  let rec processRows = (pRows, wRows, rowNum) =>
    switch (pRows, wRows, rowNum < a.gpRow) {
    | (_, [], _) => [] /* This shouldn't happen */
    | ([], w, _) => w
    | (p, [w, ...ws], true) => [w, ...processRows(p, ws, rowNum + 1)]
    | ([p, ...ps], [w, ...ws], false) => [
        processCells(p, w, 0),
        ...processRows(ps, ws, rowNum + 1)
      ]
    };
  processRows(a.gamePiece, a.well, 0)
};

let getBoard = (game) =>
  switch game {
  | Over(o) => o.well
  | Active(a) => getActBoard(a)
  };

let isGameValid = (game) => {
  let rec testCells = (pCells, wCells, colNum) =>
    switch (pCells, wCells, colNum < game.gpCol) {
    | ([], _, _) => true
    | (_, [], _) => false
    | (p, [_, ...ws], true) => testCells(p, ws, colNum + 1)
    | ([Blank, ...ps], [_, ...ws], false) => testCells(ps, ws, colNum + 1)
    | ([_, ...ps], [Blank, ...ws], false) => testCells(ps, ws, colNum + 1)
    | ([Full(_), ..._], [Full(_), ..._], false) => false
    };
  let rec testRows = (pRows, wRows, rowNum) =>
    switch (pRows, wRows, rowNum < game.gpRow) {
    | ([], _, _) => true
    | (_, [], _) => false
    | (p, [_, ...ws], true) => testRows(p, ws, rowNum + 1)
    | ([p, ...ps], [w, ...ws], false) => testCells(p, w, 0) && testRows(ps, ws, rowNum + 1)
    };
  game.gpCol >= 0
  && game.gpRow >= 0
  && game.gpCol <= (wellCols
  - List.length(List.hd(game.gamePiece)))
  && game.gpRow <= wellRows
  - List.length((game.gamePiece))
  && testRows(game.gamePiece, game.well, 0)
};

let spawn = (actGame: activeGame) =>
  Active({...actGame, well: getActBoard(actGame), gamePiece: z, gpRow: 0, gpCol: 0});

  let rec transpose = (ls) =>
  switch ls {
  | [] => []
  | [[], ..._] => []
  | ls => [List.map(List.hd, ls), ...transpose(List.map(List.tl, ls))]
  };

let act = (game, action) => {
  let potGame =
    switch (game, action) {
    | (Over(o), _) => Over(o)
    | (Active(g), Left) => Active({...g, gpCol: g.gpCol - 1})
    | (Active(g), Right) =>
      Active({...g, gpCol: g.gpCol + 1})
    | (Active(g), TurnRight) => Active({...g, gamePiece: g.gamePiece |> List.rev |> transpose })
    | (Active(g), Down) =>
      Active({...g, gpRow: g.gpRow + 1})
    };
  switch (potGame, action) {
  | (Over(o), _) => Over(o)
  | (Active(g), Down) when ! isGameValid(g) => game
  | (Active(g), _) when ! isGameValid(g) => game
  | (Active(_), _) => potGame
  }
};
