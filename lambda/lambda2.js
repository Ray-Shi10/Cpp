debug = console.log.bind(console);
dir = console.dir.bind(console);
isString = a => typeof a == 'string' || a instanceof String;
isArray = a => Array.isArray(a);
const ownKeys = obj => Object.getOwnPropertyNames(obj);
const assign = (obj, ...args) => {
  for(const arg of args) {
    for(const key of ownKeys(arg)) {
      obj[key] = arg[key];
    }
  }
  return obj;
};
const clone = function(a) {
  if(typeof a != 'object') {
    return a;
  }
  const obj = Object.create(Object.getPrototypeOf(a));
  for(const key of ownKeys(a)) {
    obj[key] = clone(a[key]);
  }
  return obj;
};
Array.prototype.first = function(func) {
  for(let i=0; i < this.length; i++) {
    if(func(this[i])) return i;
  }
};
const Lambda = (() => {
  const Base = (obj) => {
    if(obj === undefined) obj = {type: 'Base'};
    const res = Object.setPrototypeOf(assign(function(...args) {
      if(args.length == 0) return res.exec();
      let lambda = res;
      for(let arg of args) {
        if(isString(arg)) arg = Lambda(arg);
        if(isArray(arg)) arg = Lambda(...arg);
        if(arg instanceof Lambda && lambda instanceof Lambda) {
          lambda = lambda.exec(arg);
          continue;
        } else if(lambda instanceof Lambda) {
          lambda = eval((lambda+'').replaceAll('.','=>'));
        }
        lambda = lambda(arg);
      }
      return lambda;
    }, obj), Lambda[obj.type+''].prototype);
    // delete res.prototype;
    delete res.length;
    delete res.name;
    // dir(clone(res));
    return res;
  };
  Base.prototype = {
    replace(key, val) {
      return Lambda(this._replace(key, Lambda(val)) ?? this);
    },
    _replace() { throw "must be overrided"; },
    exec() { return this; },
    toString() { throw "must be overrided"; },
    toInt() {
      return this(a=>a+1, 0);
    },
    toBool() {
      return this(true, false);
    },
    res(...args) { return [...(args.length?this(...args):this)].at(-1); },
    constructor: Base,
    [Symbol.toStringTag]: 'Lambda',
    [Symbol.iterator]: function*() {
      try {
        let lambda = this;
        let last = null;
        while(lambda != last) {
          yield lambda;
          last = lambda+'';
          lambda = lambda.exec();
        }
      } catch(e) { }
    },
  };
  const extend = (Class, Base) => {
    // dir(clone(Class));
    const name = Class.name ??= Class.prototype[Symbol.toStringTag];
    Object.setPrototypeOf(Class.prototype, Base.prototype);
    Class.prototype.constructor = Class;
    const TYPE = Symbol(name);
    Class.prototype[Symbol.toStringTag] = Base.prototype[Symbol.toStringTag] + '.' + name;
    return assign((...args) => {
      const res = assign(Object.setPrototypeOf(Base(), Class.prototype), {type: TYPE}, new Class(...args));
      delete res.prototype; return res;
    }, Class);
  };
  const List = extend(class List {
    constructor(...items) {
      if(items.length == 1) { null.a; }
      assign(this, items.map(a=>Lambda(a)));
    }
    get _list() { return Array.from(assign({},this)); }
    // slice(pos) {
    //   const res = [];
    //   while(pos < this.length) {
    //     res.push(this[pos]);
    //     pos++;
    //   }
    //   return res;
    // }
    _replace(key, val) {
      return Lambda(...this._list.map(a=>a.replace(key,val)));
    }
    toString() {
      return this._list.map((a,i) => a instanceof Token && !i ? a : '('+a+')').join('');
    }
    exec(val) {
      if(val === undefined) {
        if(this._list[0] instanceof Func) {
          return Lambda(this._list[0].exec(this._list[1]), ...this._list.slice(2));
        } else {
          for(let i=0; i<this.length; i++) {
            if(this._list[i] instanceof Token) continue;
            return Lambda(...this._list.slice(0,i), this._list[i].exec(), ...this._list.slice(i+1));
          }
          return this;
          null.a;
        }
      }
      return Lambda(...this._list, val);
    }
  }, Base);
  const Token = extend(class Token {
    constructor(str) {
      this.str = str+'';
    }
    _replace(key, val) {
      if(key == this+'') return val;
    }
    toString() {
      return this.str;
    }
  }, Base);
  const Func = extend(class Func {
    constructor(arg, body) {
      this.arg = arg+'';
      this.body = Lambda(body);
    }
    _replace(key, val) {
      if(key !== this.arg) {
        return Lambda.Func(this.arg, this.body.replace(key, val));
      }
    }
    toString() {
      return `${this.arg}.${this.body}`;
    }
    exec(arg) {
      if(arg === undefined) return Lambda.Func(this.arg, this.body.exec());
      return this.body.replace(this.arg, arg);
    }
  }, Base);
  const Lambda = (f => (...args)=>{
    if(args.length > 1) return List(...args);
    const arg = args[0];
    if(typeof arg == 'object') return Base(arg);
    if(typeof arg == 'function') return (arg);
    if(typeof arg == 'number') {
      const n = +arg;
      return Func('f', Func('x', 'f('.repeat(n)+'x'+')'.repeat(n)));
    }
    if(typeof arg == 'boolean') {
      return Func('x', Func('y', arg ? 'x' : 'y'));
    }
    return f(arg);
  })((str) => {
    if(/^\w+\./.test(str)) {
      const arg = str.split('.')[0];
      const body = str.slice(arg.length + 1);
      return Func(arg, body);
    } else {
      let braket = 0;
      let vals = [""];
      for(let i = 0; i < str.length; i++) {
        const c = str[i];
        // debug(str, i, c, braket);
        if(c == '(') {
          if(braket == 0) {
            vals.push("");
            braket++;
            continue;
          }
          braket++;
        }
        else if(c == ')') {
          braket--;
          if(braket == 0) {
            vals.push("");
            continue;
          }
        }
        // if(!' \t\n\r)'.split('').includes(c) && braket == 0) {
        //   vals.push("");
        // } else {
          vals[vals.length-1] += c;
        // }
      }
      // debug('Vals:', vals);
      vals = vals.map(a => a.split(/\s+/)).flat(1);
      if(vals.length == 1) {
        const str = vals[0];
        if(/^\d+$/.test(str)) {
          const n = +str;
          return Func('f', Func('x', 'f('.repeat(n)+'x'+')'.repeat(n)));
        }
        // debug(str)
        // if('pair true false'.split(' ').includes(str)) {
        //   return clone(Lambda['__'+str]);
        // }
        return Token(str);
      }
      vals = vals.filter(Boolean);
      // vals = vals.map(Lambda);
      // debug('List:', vals);
      return Lambda(...vals);
    }
  });
  Lambda.Base = Base;
  Lambda.Func = Func;
  Lambda.List = List;
  Lambda.Token = Token;
  Lambda.prototype = Base.prototype;
  return Lambda;
})();
U = Lambda('f.f(f)','u.f.f(u(u)(f))')()();
pre = Lambda('n.f.x.n(a.a(x.y.x)(_._(x.y.x)(f(a(x.y.y))))(_._(x.y.x)(a(x.y.y))))(_._(x.y.y)(x))(x.y.y)');
mul = Lambda('m.n.f.n(m(f))');
frac = U(['mul.pre.f.n.n(a.x.y.y)(x.y.x)(1)(mul(n)(f(pre(n))))', mul, pre]);
// frac = Lambda(U,'F.n.n(a.x.y.y)(x.y.x)(1)(f.n(F(f.x.n(a.a(x.y.x)(_._(x.y.x)(f(a(x.y.y))))(_._(x.y.x)(a(x.y.y))))(_._(x.y.y)(x))(x.y.y))(f))')
frac = Lambda('n.n(a._._(f.x.f(a(x.y.x)(f)(x)))(mul(a(x.y.x))(a(x.y.y))))(_.1)(x.y.y)');
const a = frac('10');
const start = Date.now();
debug(a.res().toInt());
debug('Time:', (Date.now()-start)/1000, 's');
// // for(const step of a) {
// //   debug(step+'');
// // }