[%bs.raw {|require('./Board.css')|}];

let component = ReasonReact.statelessComponent("Board");

let renderCell = (cell) => {
  let c =
    switch cell {
    | Tetris.Blank => "blank"
    | Full(I) => "I"
    | Full(J) => "J"
    | Full(L) => "L"
    | Full(O) => "O"
    | Full(S) => "S"
    | Full(T) => "T"
    | Full(Z) => "Z"
    };
  <div className={j|cell cell-$c|j} />
};

let make = (~board, ~className, ~isPaused, _children) => {
  ...component,
  render: (_self) =>
    <div className>
      (
        board
        |> List.rev
        |> List.flatten
        |> List.map((cell) => isPaused ? <div className="cell cell-paused" /> : renderCell(cell))
        |> Array.of_list
        |> ReasonReact.arrayToElement
      )
    </div>
};
