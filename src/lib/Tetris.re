let wellRows = 20;

let wellCols = 10;

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

let i = [
  [Blank, Blank, Blank, Blank],
  [Full(I), Full(I), Full(I), Full(I)],
  [Blank, Blank, Blank, Blank],
  [Blank, Blank, Blank, Blank]
];

let j = [[Full(J), Blank, Blank], [Full(J), Full(J), Full(J)], [Blank, Blank, Blank]];

let l = [[Blank, Blank, Full(L)], [Full(L), Full(L), Full(L)], [Blank, Blank, Blank]];

let o = [[Full(O), Full(O)], [Full(O), Full(O)]];

let s = [[Blank, Full(S), Full(S)], [Full(S), Full(S), Blank], [Blank, Blank, Blank]];

let t = [[Blank, Blank, Blank], [Full(T), Full(T), Full(T)], [Blank, Full(T), Blank]];

let z = [[Full(Z), Full(Z), Blank], [Blank, Full(Z), Full(Z)], [Blank, Blank, Blank]];

type well = list(list(cell));

type tetroid = list(list(cell));

type action =
  | Left
  | Right
  | TurnLeft
  | TurnRight
  | Down
  | HardDrop;

type activeGame = {
  well,
  lines: int,
  level: int,
  score: int,
  tetroid,
  tBottom: int,
  tLeft: int,
  bag: list(tetroid)
};

type overGame = {
  well,
  lines: int,
  level: int,
  score: int
};

type game =
  | Active(activeGame)
  | Over(overGame);

let validateGame = (game) => {
  let rec testCells = (tCells, wCells, col) =>
    switch (tCells, wCells, col < game.tLeft, col < 0) {
    | ([Blank, ...ts], w, _, true) =>
      /* left of Well w/ Blank Tetroid*/
      testCells(ts, w, col + 1)
    | ([Full(_), ..._], _, _, true) =>
      /* left of Well w/ Full Tetroid*/
      false
    | ([Blank, ...ts], [], _, _) =>
      /* right of Well w/ Blank Tetroid*/
      testCells(ts, [], col + 1)
    | ([Full(_), ..._], [], _, _) =>
      /* right of Well w/ Full Tetroid*/
      false
    | (t, [_, ...ws], true, false) =>
      /* left of Tetroid */
      testCells(t, ws, col + 1)
    | ([], _, _, _) =>
      /* right of Tetroid */
      true
    | ([Blank, ...ts], [_, ...ws], false, false) =>
      /* Within Tetroid - Tetroid is blank */
      testCells(ts, ws, col + 1)
    | ([_, ...ts], [Blank, ...ws], false, false) =>
      /* Within Tetroid - Well is blank */
      testCells(ts, ws, col + 1)
    | ([Full(_), ..._], [Full(_), ..._], false, false) =>
      /* Within Tetroid - Well and Tetroid are Full */
      false
    };
  let rowIsBlank = List.for_all((cell) => cell === Blank);
  let rec testRows = (tRows, wRows, row) =>
    switch (tRows, wRows, row < game.tBottom, row < 0) {
    | ([t, ...ts], w, _, true) =>
      /* below Well */
      rowIsBlank(t) && testRows(ts, w, row + 1)
    | ([_, ...ts], [], _, _) =>
      /* above Well */
      testRows(ts, [], row + 1)
    | (t, [_, ...ws], true, false) =>
      /* below Tetroid */
      testRows(t, ws, row + 1)
    | ([], _, _, _) =>
      /* above Tetroid */
      true
    | ([t, ...ts], [w, ...ws], false, false) =>
      /* within tetroid row */
      testCells(t, w, min(game.tLeft, 0)) && testRows(ts, ws, row + 1)
    };
  testRows(game.tetroid, game.well, min(game.tBottom, 0))
};

let genBag = () => {
  let ag = [|i, j, l, o, s, t, z|];
  let n = Array.length(ag);
  for (i in n - 1 downto 1) {
    let k = Js.Math.random_int(0, i + 1);
    let x = ag[k];
    ag[k] = ag[i];
    ag[i] = x
  };
  ag |> Array.to_list
};

let spawn = (ag: activeGame) => {
  let bag = ag.bag === [] ? genBag() : ag.bag;
  let tetroid = List.hd(bag);
  let newBag = List.tl(bag);
  let newAg = {
    ...ag,
    tetroid,
    tBottom: 22 - List.length(tetroid),
    tLeft: (wellCols - List.(length(hd(tetroid)))) / 2,
    bag: newBag
  };
  validateGame(newAg) ?
    Active(newAg) : Over({well: ag.well, lines: ag.lines, level: ag.level, score: ag.score})
};

let init = () =>
  spawn({
    well: Array.(make_matrix(wellRows, wellCols, Blank) |> map(to_list) |> to_list),
    lines: 0,
    level: 1,
    score: 0,
    tetroid: i,
    tBottom: 0,
    tLeft: 0,
    bag: genBag()
  });

let getOverlaidBoard = (ag) => {
  let rec getCells = (tCells, wCells, col) =>
    switch (tCells, wCells, col < ag.tLeft, col < 0) {
    | ([_, ...ts], w, _, true) =>
      /* left of Well */
      getCells(ts, w, col + 1)
    | (_, [], _, _) =>
      /* right of Well */
      []
    | (t, [w, ...ws], true, false) =>
      /* left of Tetroid */
      [w, ...getCells(t, ws, col + 1)]
    | ([], w, _, _) =>
      /* right of Tetroid */
      w
    | ([Blank, ...ts], [w, ...ws], false, false) =>
      /* Within Tetroid - Tetroid is blank */
      [w, ...getCells(ts, ws, col + 1)]
    | ([Full(t), ...ts], [_, ...ws], false, false) =>
      /* Within Tetroid - Tetroid is Full */
      [Full(t), ...getCells(ts, ws, col + 1)]
    };
  let rec getRows = (tBottoms, wRows, row) =>
    switch (tBottoms, wRows, row < ag.tBottom, row < 0) {
    | ([_, ...ts], w, _, true) =>
      /* below Well */
      getRows(ts, w, row + 1)
    | (_, [], _, _) =>
      /* above Well */
      []
    | (t, [w, ...ws], true, false) =>
      /* below Tetroid */
      [w, ...getRows(t, ws, row + 1)]
    | ([], w, _, _) =>
      /* above Tetroid */
      w
    | ([t, ...ts], [w, ...ws], false, false) =>
      /* within tetroid row */
      [getCells(t, w, min(ag.tLeft, 0)), ...getRows(ts, ws, row + 1)]
    };
  getRows(ag.tetroid, ag.well, min(ag.tBottom, 0))
};

let getBoard = (game) =>
  switch game {
  | Over(og) => og.well
  | Active(ag) => getOverlaidBoard(ag)
  };

let rec transpose = (ls) =>
  switch ls {
  | [] => []
  | [[], ..._] => []
  | ls => [List.map(List.hd, ls), ...transpose(List.map(List.tl, ls))]
  };

let all = (x) => {
  let rec xs = [x, ...xs];
  xs
};

let rec take = (n, ls) =>
  switch (n, ls) {
  | (_, []) => []
  | (0, _) => []
  | (_, [x, ...xs]) => [x, ...take(n - 1, xs)]
  };

let padTrim = (e, n, ls) => take(n, ls @ all(e));

let act = (game, action) =>
  switch game {
  | Over(og) => Over(og)
  | Active(ag) =>
    let potGame =
      switch action {
      | Left => {...ag, tLeft: ag.tLeft - 1}
      | Right => {...ag, tLeft: ag.tLeft + 1}
      | TurnLeft => {...ag, tetroid: ag.tetroid |> List.rev |> transpose}
      | TurnRight => {...ag, tetroid: ag.tetroid |> transpose |> List.rev}
      | Down => {...ag, tBottom: ag.tBottom - 1}
      | HardDrop =>
        let rec findBottom = (validAg) => {
          let nextAg = {...validAg, tBottom: validAg.tBottom - 1};
          validateGame(nextAg) ? findBottom(nextAg) : validAg
        };
        findBottom(ag)
      };
    switch (validateGame(potGame), action) {
    | (true, _) => Active(potGame)
    | (false, Left | Right | TurnLeft | TurnRight) => game
    | (false, HardDrop | Down) =>
      let packedWell = getOverlaidBoard(ag) |> List.filter(List.exists((cell) => cell === Blank));
      let lines = wellRows - List.length(packedWell);
      let level = ag.lines + lines <= ag.level * 3 ? ag.level : ag.level + 1;
      let score =
        ag.score
        + level
        * (
          switch lines {
          | 1 => 100
          | 2 => 300
          | 3 => 500
          | 4 => 800
          | _ => 0 /* shouldn't happen */
          }
        );
      let well = packedWell |> padTrim(Array.(make(wellCols, Blank) |> to_list), wellRows);
      spawn({...ag, well, level, lines: ag.lines + lines, score})
    }
  };

let getLevel = (game) =>
  switch game {
  | Active(ag) => ag.level
  | Over(og) => og.level
  };

let getScore = (game) =>
  switch game {
  | Active(ag) => ag.score
  | Over(og) => og.score
  };

let getLines = (game) =>
  switch game {
  | Active(ag) => ag.lines
  | Over(og) => og.lines
  };

let isActive = (game) =>
  switch game {
  | Active(_) => true
  | Over(_) => false
  };
