[%bs.raw {|require('./App.css')|}];

type state = {
  game: Tetris.game,
  timerId: ref(option(Js.Global.intervalId))
};

type action =
  | Tick
  | UserEvent(EventLayer.direction)
  | Restart;

let component = ReasonReact.reducerComponent("App");

let make = (_children) => {
  ...component,
  initialState: () => {game: Tetris.init(), timerId: ref(None)},
  didMount: (self) => {
    self.state.timerId := Some(Js.Global.setInterval(self.reduce((_) => Tick), 500));
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
        <h2> (ReasonReact.stringToElement("Lines")) </h2>
        <h3> (ReasonReact.stringToElement("0")) </h3>
        <h2> (ReasonReact.stringToElement("Level")) </h2>
        <h3> (ReasonReact.stringToElement("0")) </h3>
        <h2> (ReasonReact.stringToElement("Score")) </h2>
        <h3> (ReasonReact.stringToElement("0")) </h3>
      </div>
      <GameOver
        gameOver=(
          switch state.game {
          | Over(_) => true
          | Active(_) => false
          }
        )
        score=0
        onReplay=(reduce((_event) => Restart))
      />
    </EventLayer>
};
