[%bs.raw {|require('./Help.css')|}];

let row = (keys, action) =>
  ReasonReact.arrayToElement([|
    <div className="keys"> (ReasonReact.stringToElement(keys)) </div>,
    <div className="colon"> (ReasonReact.stringToElement(":")) </div>,
    <div className="action"> (ReasonReact.stringToElement(action)) </div>
  |]);

let component = ReasonReact.statelessComponent("Help");

let make = (~visible, _children) => {
  ...component,
  render: (_self) =>
    switch visible {
    | false => ReasonReact.nullElement
    | true =>
      <div className="help">
        <h2> (ReasonReact.stringToElement("Help")) </h2>
        <div className="helpGrid">
          (row("Arrow Left / Right", "Move Left / Right"))
          (row("X, Arrow Up", "Rotate Right"))
          (row("Z, CTRL", "Rotate Left"))
          (row("Arrow Down", "Fast Drop"))
          (row("SPACE", "Hard Drop"))
          <div className="keys" />
          (row("H", "Help"))
          (row("M", "Mute"))
        </div>
      </div>
    }
};
