## [Play Here!](https://sevenseat.github.io/re-tetris)

This is a simplified version of Tetris, written in [Reason React](https://reasonml.github.io/reason-react/). Nothing fancy, just a bit of fun. It is my second toy Reason React application, after [2048](https://sevenseat.github.io/rr-2048)

## Key Observations

* I continue to be amazed how much i can do in so-little code. The reason-react components are trivial. The Tetris game logic is a bit more complex, but its still ~250 lines (of which 75 are type definitions).
* I may have gotten too cute with the type-system, that caused a bit of code ugliness. I declared a Variant of the Game state with two values. Over(), and Active()..... The intention was to make all the function calls that would change game-state only work on Active() games. However, it had some inintended consequences. Over() and Active() had a bunch of common values (e.g., score, level). I found myself having to write a SWITCH statement to unwrap the Game, then access the same values in each. Not the worst thing in the world, but duplicative code and ugly. I wonder if theres a feature that would allow me to access the common properties of each variant without the switch statement.
* The standard library is disappointing. Most of the game logic leverages Linked Lists, which are idomatic in Reason/OCAML. But the standard library is missing a lot of functions that I would have expected, like flatmap, transpose List(List(`a)), pad_end, etc... I ended up copying code over from my [2048](https://sevenseat.github.io/rr-2048) game for these simple functions. I think Reason React has a roadmap for more powerful datastructures. I can't wait
* fold_left and fold_right still scare me. I'm never able to figure them out by their type signatures. So I revert to recursion.

## Todos

* [x] Levels increase speed of game tick
* [ ] Hard drop
* [ ] Soft drop increases game tick (rather than moves it down 1 cell)
* [ ] [SRS](http://tetris.wikia.com/wiki/SRS) - MAYBE.... It's a lot of logic
* [x] Beautify CSS
* [x] Mobile friendly
* [x] Get rid of absolute sizing within the Board component
