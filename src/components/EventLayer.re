type touchData = {
  x: float,
  y: float,
  time: float
};

type direction =
  | Left
  | Right
  | Up
  | Down;

type action =
  | TouchStart(touchData)
  | TouchEnd(touchData);

type state = {
  touchStart: option(touchData),
  elementRef: ref(option(Dom.element))
};

let component = ReasonReact.reducerComponent("EventLayer");

let getTouchData = (touchesType, event) => {
  let touch = touchesType(event)##item(0);
  {x: touch##screenX, y: touch##screenY, time: Js.Date.now()}
};

let getGesture = (start_, end_) => {
  let min = 4000.0;
  let speedDown = (end_.y -. start_.y) *. 10000.0 /. (end_.time -. start_.time);
  let speedRight = (end_.x -. start_.x) *. 10000.0 /. (end_.time -. start_.time);
  let isHoriz = abs_float(speedRight) > abs_float(speedDown);
  switch (
    isHoriz,
    (abs_float(speedRight) -. min > 0.0, speedRight > 0.0),
    (abs_float(speedDown) -. min > 0.0, speedDown > 0.0)
  ) {
  | (true, (true, true), _) => Some(Right)
  | (true, (true, false), _) => Some(Left)
  | (false, _, (true, true)) => Some(Down)
  | (false, _, (true, false)) => Some(Up)
  | _ => None
  }
};

let setElemRef = (theRef, {ReasonReact.state}) => state.elementRef := Js.Nullable.to_opt(theRef);

/* wondering about Js.Nullable.to_opt? See the note below */
let make = (~onAction, ~className=?, children) => {
  ...component,
  initialState: () => {touchStart: None, elementRef: ref(None)},
  didMount: (self) =>
    switch self.state.elementRef^ {
    | None => ReasonReact.NoUpdate
    | Some(field) =>
      ignore(ReactDOMRe.domElementToObj(field)##focus());
      ReasonReact.NoUpdate
    },
  reducer: (action, state) =>
    switch (action, state.touchStart) {
    | (TouchStart(td), _) => ReasonReact.Update({...state, touchStart: Some(td)})
    | (TouchEnd(_td), None) => ReasonReact.NoUpdate
    | (TouchEnd(td), Some(start)) =>
      switch (getGesture(start, td)) {
      | Some(direction) =>
        ReasonReact.UpdateWithSideEffects(
          {...state, touchStart: None},
          ((_self) => onAction(direction))
        )
      | None => ReasonReact.NoUpdate
      }
    },
  render: ({reduce, handle}) =>
    <div
      onTouchStart=(reduce((e) => TouchStart(getTouchData(ReactEventRe.Touch.targetTouches, e))))
      onTouchEnd=(reduce((e) => TouchEnd(getTouchData(ReactEventRe.Touch.changedTouches, e))))
      onTouchMove=((e) => ReactEventRe.Touch.preventDefault(e))
      ?className
      tabIndex=0
      ref=(handle(setElemRef))>
      (ReasonReact.arrayToElement(children))
    </div>
};
