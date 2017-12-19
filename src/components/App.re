[%bs.raw {|require('./App.css')|}];

type state = {game: Tetris.game};

type action =
  | UserEvent(EventLayer.direction)
  | Restart;

let component = ReasonReact.reducerComponent("App");

let make = (_children) => {
  ...component,
  initialState: () => {game: Tetris.init()},
  reducer: (action, state) =>
    switch action {
    | UserEvent(EventLayer.Left) => ReasonReact.Update({game: Tetris.act(state.game, Tetris.Left)})
    | UserEvent(EventLayer.Right) =>
      ReasonReact.Update({game: Tetris.act(state.game, Tetris.Right)})
    | UserEvent(EventLayer.Up) =>
      ReasonReact.Update({game: Tetris.act(state.game, Tetris.TurnRight)})
    | UserEvent(EventLayer.Down) => ReasonReact.Update({game: Tetris.act(state.game, Tetris.Down)})
    | Restart => ReasonReact.NoUpdate
    },
  render: ({state, reduce}) =>
    <EventLayer className="App" onAction=(reduce((direction) => UserEvent(direction)))>
      <Board board=(Tetris.getBoard(state.game)) />
    </EventLayer>
};