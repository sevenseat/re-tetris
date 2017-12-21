[%bs.raw {|require('./App.css')|}];

type state = {
  game: Tetris.game,
  timerId: ref(option(Js.Global.intervalId))
};

type action =
  | Tick
  | UserEvent(EventLayer.direction)
  | Restart;

let elementState = (label, value) =>
  <div>
    <h2> (ReasonReact.stringToElement(label)) </h2>
    <h3> (ReasonReact.stringToElement(string_of_int(value))) </h3>
  </div>;

let component = ReasonReact.reducerComponent("App");

let make = (_children) => {
  ...component,
  initialState: () => {game: Tetris.init(), timerId: ref(None)},
  didMount: ({state, reduce}) => {
    let prevLevel = float_of_int(Tetris.getLevel(state.game) - 1);
    state.timerId :=
      Some(
        Js.Global.setInterval(
          reduce((_) => Tick),
          int_of_float(1000.0 *. ((0.8 -. 0.007 *. prevLevel) ** prevLevel))
        )
      );
    ReasonReact.NoUpdate
  },
  reducer: (action, state) =>
    switch action {
    | UserEvent(direction) =>
      ReasonReact.Update({
        ...state,
        game:
          Tetris.act(
            state.game,
            switch direction {
            | EventLayer.Left => Tetris.Left
            | EventLayer.Right => Tetris.Right
            | EventLayer.Up => Tetris.TurnRight
            | EventLayer.Down => Tetris.Down
            }
          )
      })
    | Tick => ReasonReact.Update({...state, game: Tetris.act(state.game, Tetris.Down)})
    | Restart => ReasonReact.Update({...state, game: Tetris.init()})
    },
  render: ({state, reduce}) =>
    <EventLayer className="App" onAction=(reduce((direction) => UserEvent(direction)))>
      <h1 className="Title"> (ReasonReact.stringToElement("Re-Tetris")) </h1>
      <Board className="board" board=(Tetris.getBoard(state.game)) />
      <div className="Game-info">
        (elementState("Lines", Tetris.getLines(state.game)))
        (elementState("Level", Tetris.getLevel(state.game)))
        (elementState("Score", Tetris.getScore(state.game)))
      </div>
      <GameOver
        gameOver=(! Tetris.isActive(state.game))
        score=(Tetris.getScore(state.game))
        onReplay=(reduce((_event) => Restart))
      />
    </EventLayer>
};
