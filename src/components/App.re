[%bs.raw {|require('./App.css')|}];

let theme: string = [%bs.raw {|require('../sound/theme.mp3')|}];

type state = {
  game: Tetris.game,
  softDrop: bool,
  muted: bool,
  timerId: ref(option(Js.Global.intervalId))
};

type action =
  | Tick
  | KeyDown(int)
  | KeyUp(int)
  | Swipe(EventLayer.direction)
  | Restart;

let elementState = (label, value) =>
  <div>
    <h2> (ReasonReact.stringToElement(label)) </h2>
    <h3> (ReasonReact.stringToElement(string_of_int(value))) </h3>
  </div>;

let takeAction = (state, action) =>
  ReasonReact.Update({...state, game: Tetris.act(state.game, action)});

let setTimer = (game, softDrop, reduce) => {
  let prevLevel = float_of_int(Tetris.getLevel(game) - 1);
  Some(
    Js.Global.setInterval(
      reduce((_) => Tick),
      int_of_float(1000.0 /. (softDrop ? 20.0 : 1.0) *. ((0.8 -. 0.007 *. prevLevel) ** prevLevel))
    )
  )
};

let component = ReasonReact.reducerComponent("App");

let make = (_children) => {
  ...component,
  initialState: () => {game: Tetris.init(), softDrop: false, muted: false, timerId: ref(None)},
  didMount: (self) => {
    self.state.timerId := setTimer(self.state.game, self.state.softDrop, self.reduce);
    ReasonReact.NoUpdate
  },
  didUpdate: ({oldSelf, newSelf}) =>
    Tetris.getLevel(oldSelf.state.game) === Tetris.getLevel(newSelf.state.game)
    && oldSelf.state.softDrop === newSelf.state.softDrop ?
      () :
      {
        switch oldSelf.state.timerId^ {
        | None => ()
        | Some(intervalId) => Js.Global.clearInterval(intervalId)
        };
        newSelf.state.timerId :=
          setTimer(newSelf.state.game, newSelf.state.softDrop, newSelf.reduce)
      },
  reducer: (action, state) =>
    switch action {
    | Swipe(EventLayer.Left) => takeAction(state, Tetris.Left)
    | Swipe(EventLayer.Right) => takeAction(state, Tetris.Right)
    | Swipe(EventLayer.Up) => takeAction(state, Tetris.TurnRight)
    | Swipe(EventLayer.Down) => takeAction(state, Tetris.HardDrop)
    | KeyDown(37 | 100) => takeAction(state, Tetris.Left)
    | KeyDown(39 | 102) => takeAction(state, Tetris.Right)
    | KeyDown(40 | 98) => ReasonReact.Update({...state, softDrop: true})
    | KeyDown(32 | 104) => takeAction(state, Tetris.HardDrop)
    | KeyDown(38 | 88 | 97 | 101 | 105) => takeAction(state, Tetris.TurnRight)
    | KeyDown(17 | 90 | 99 | 103) => takeAction(state, Tetris.TurnLeft)
    | KeyDown(77) => ReasonReact.Update({...state, muted: ! state.muted})
    | KeyDown(_) => ReasonReact.NoUpdate
    | KeyUp(40 | 98) => ReasonReact.Update({...state, softDrop: false})
    | KeyUp(_) => ReasonReact.NoUpdate
    | Tick => takeAction(state, Tetris.Down)
    | Restart => ReasonReact.Update({...state, game: Tetris.init()})
    },
  render: ({state, reduce}) =>
    <div
      onKeyDown=(reduce((e) => KeyDown(ReactEventRe.Keyboard.which(e))))
      onKeyUp=(reduce((e) => KeyUp(ReactEventRe.Keyboard.which(e))))>
      <EventLayer className="App" onAction=(reduce((direction) => Swipe(direction)))>
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
        <audio
          src=theme
          autoPlay=Js.true_
          loop=Js.true_
          muted=(Js.Boolean.to_js_boolean(state.muted))
        />
      </EventLayer>
    </div>
};
