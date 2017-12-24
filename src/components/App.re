[%bs.raw {|require('./App.css')|}];

let theme: string = [%bs.raw {|require('../media/theme.mp3')|}];

let mute: string = [%bs.raw {|require('../media/mute.svg')|}];

let unmute: string = [%bs.raw {|require('../media/unmute.svg')|}];

type state = {
  game: Tetris.game,
  softDrop: bool,
  isMuted: bool,
  isPaused: bool,
  timerId: ref(option(Js.Global.intervalId))
};

type action =
  | Tick
  | KeyDown(int)
  | KeyUp(int)
  | Swipe(EventLayer.direction)
  | ToggleSound
  | TogglePause
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
  initialState: () => {
    game: Tetris.init(),
    softDrop: false,
    isMuted: false,
    isPaused: false,
    timerId: ref(None)
  },
  didMount: (self) => {
    self.state.timerId := setTimer(self.state.game, self.state.softDrop, self.reduce);
    ReasonReact.NoUpdate
  },
  didUpdate: ({oldSelf, newSelf}) => {
    let levelChanged = Tetris.getLevel(oldSelf.state.game) !== Tetris.getLevel(newSelf.state.game);
    let softDropChanged = oldSelf.state.softDrop !== newSelf.state.softDrop;
    switch (levelChanged || softDropChanged) {
    | false => ()
    | true =>
      switch oldSelf.state.timerId^ {
      | None => ()
      | Some(intervalId) => Js.Global.clearInterval(intervalId)
      };
      newSelf.state.timerId := setTimer(newSelf.state.game, newSelf.state.softDrop, newSelf.reduce)
    }
  },
  reducer: (action, state) =>
    switch (state.isPaused, action) {
    | (true, KeyDown(_))
    | (_, TogglePause)
    | (false, KeyDown(72)) => ReasonReact.Update({...state, isPaused: ! state.isPaused})
    | (_, KeyDown(77) | ToggleSound) => ReasonReact.Update({...state, isMuted: ! state.isMuted})
    | (false, KeyDown(37 | 100) | Swipe(EventLayer.Left)) => takeAction(state, Tetris.Left)
    | (false, KeyDown(39 | 102) | Swipe(EventLayer.Right)) => takeAction(state, Tetris.Right)
    | (false, KeyDown(38 | 88 | 97 | 101 | 105) | Swipe(EventLayer.Up)) =>
      takeAction(state, Tetris.TurnRight)
    | (false, KeyDown(17 | 90 | 99 | 103)) => takeAction(state, Tetris.TurnLeft)
    | (false, KeyDown(32 | 104) | Swipe(EventLayer.Down)) => takeAction(state, Tetris.HardDrop)
    | (false, KeyDown(40 | 98)) => ReasonReact.Update({...state, softDrop: true})
    | (false, KeyUp(40 | 98)) => ReasonReact.Update({...state, softDrop: false})
    | (_, KeyDown(_) | KeyUp(_))
    | (true, Swipe(_))
    | (true, Tick) => ReasonReact.NoUpdate
    | (false, Tick) => takeAction(state, Tetris.Down)
    | (_, Restart) => ReasonReact.Update({...state, game: Tetris.init()})
    },
  render: ({state, reduce}) =>
    <div
      onKeyDown=(reduce((e) => KeyDown(ReactEventRe.Keyboard.which(e))))
      onKeyUp=(reduce((e) => KeyUp(ReactEventRe.Keyboard.which(e))))>
      <EventLayer className="App" onAction=(reduce((direction) => Swipe(direction)))>
        <h1 className="Title"> (ReasonReact.stringToElement("Re-Tetris")) </h1>
        <Board className="board" isPaused=state.isPaused board=(Tetris.getBoard(state.game)) />
        <div className="Game-info">
          (elementState("Lines", Tetris.getLines(state.game)))
          (elementState("Level", Tetris.getLevel(state.game)))
          (elementState("Score", Tetris.getScore(state.game)))
          <div className="controls">
            <a onClick=(reduce((_event) => TogglePause))>
              (ReasonReact.stringToElement("Help"))
            </a>
            <img src=(state.isMuted ? mute : unmute) onClick=(reduce((_event) => ToggleSound)) />
          </div>
        </div>
        <Help visible=state.isPaused />
        <GameOver
          gameOver=(! Tetris.isActive(state.game))
          score=(Tetris.getScore(state.game))
          onReplay=(reduce((_event) => Restart))
        />
        <audio
          src=theme
          autoPlay=Js.true_
          loop=Js.true_
          muted=(Js.Boolean.to_js_boolean(state.isMuted))
        />
      </EventLayer>
    </div>
};
