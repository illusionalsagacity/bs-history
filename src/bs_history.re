module type State = {type t;};

/** @since 0.1.0 */
module Make = (State: State) => {
  type action = [ | `PUSH | `REPLACE | `POP];

  type location = {
    pathname: string,
    search: string,
    hash: string,
    key: Js.nullable(string),
    state: Js.nullable(State.t),
  };

  module PartialLocation = {
    type t = {
      pathname: Js.nullable(string),
      search: Js.nullable(string),
      hash: Js.nullable(string),
      key: Js.nullable(string),
      state: Js.nullable(State.t),
    };

    [@bs.obj]
    external make:
      (~pathname: string=?, ~search: string=?, ~hash: string=?, ~key: string=?, ~state: State.t=?, unit) => t;
  };

  type t = {
    length: int,
    action,
    location,
    index: Js.nullable(int) // FIXME: This only exists on MemoryHistory
  };

  /** @since 0.1.0 */
  module PartialPath = {
    type t = {
      pathname: Js.nullable(string),
      search: Js.nullable(string),
      hash: Js.nullable(string),
    };

    [@bs.obj] external make: (~pathname: string=?, ~search: string=?, ~hash: string=?, unit) => t;
  };

  /** @since 0.1.0 */
  module Transition = {
    type t = {
      action,
      location,
    };

    [@bs.send] external retry: t => unit = "retry";
  };

  /** @since 0.1.0 */
  type update = {
    action,
    location,
  };

  [@bs.send] external listen: (t, (. update) => unit) => (. unit) => unit = "listen";
  [@bs.send]
  external push: (t, [@bs.unwrap] [ | `str(string) | `to_(PartialPath.t)], ~state: State.t=?) => unit = "push";
  [@bs.send]
  external replace: (t, [@bs.unwrap] [ | `str(string) | `to_(PartialPath.t)], ~state: State.t=?) => unit = "replace";
  [@bs.send] external go: (t, int) => unit = "go";
  [@bs.send] external goBack: t => unit = "goBack";
  [@bs.send] external goForward: t => unit = "goForward";
  [@bs.send] external block: (t, (. Transition.t) => unit) => (. unit) => unit = "block";
  [@bs.send] external createHref: (t, [@bs.unwrap] [ | `str(string) | `to_(PartialPath.t)]) => string = "createHref";
};

module State = {
  type t;
};

/**
 * A convenient default history with a state phantom type, in case you don't care about using location state.
 */
module History = Make(State);

/** @since 0.1.0 */
module BrowserHistoryOptions = {
  type t = {window: Js.nullable(Dom.window)};

  [@bs.obj] external make: (~window: Dom.window=?, unit) => t;
};

/** @since 0.1.0 */ [@bs.module] external createPath: History.PartialPath.t => string = "createPath";

/** @since 0.1.0 */ [@bs.module] external parsePath: string => History.PartialPath.t = "parsePath";

[@bs.module "history"]
external createBrowserHistory: (~options: BrowserHistoryOptions.t=?, unit) => History.t = "createBrowserHistory";

/** @since 0.1.0 */
module MemoryHistoryOptions = {
  [@unboxed]
  type entry =
    | Entry('a): entry;

  type t = {
    initialEntries: Js.nullable(array(entry)),
    initialIndex: Js.nullable(int),
  };

  [@bs.obj] external make: (~initialEntries: array(entry)=?, ~initialIndex: int=?, unit) => t;
};

[@bs.module "history"]
external createMemoryHistory: (~options: MemoryHistoryOptions.t=?, unit) => History.t = "createMemoryHistory";

/** @since 0.1.0 */
module HashHistoryOptions = {
  type t = {window: Js.nullable(Dom.window)};

  [@bs.obj] external make: (~window: Dom.window=?, unit) => t;
};

/** @since 0.1.0 */
[@bs.module "history"]
external createHashHistory: (~options: HashHistoryOptions.t=?, unit) => History.t = "createHashHistory";
