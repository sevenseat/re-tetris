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
    | Restart => ReasonReact.NoUpdate
    },
  render: ({state, reduce}) =>
    <EventLayer className="App" onAction=(reduce((direction) => UserEvent(direction)))>
      <Board board=(Tetris.getBoard(state.game)) />
    </EventLayer>
};