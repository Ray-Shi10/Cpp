compile = s => (s[0]
  .replaceAll(/[^\w]\d+/g, (f=>n=>n[0]+f(n.slice(1)))(n=>`f.x.${"f(".repeat(n)}x${")".repeat(n)}`))
  .replaceAll('true', 'x.y.x').replaceAll('false', 'x.y.y')
  .replaceAll('[', '_$_._$_(').replaceAll(']', ')')
  // .replaceAll('.', '=>')
  .replaceAll(',', ')(')
);
run = s => eval((`
    not = a.a(false,true);
    and = a.b.a(b,false);
    or  = a.b.a(true,b);
    xor = a.b.a(not(b),b);
    is0 = n.n(f.x.true, false);
    pre = n.f.x.n(a.a(true)([true,f(a(false))], [true,a(false)]), [false,x])(false);
    add = m.n.f.x.n(f,m(f,x));
    mul = m.n.f.n(m(f));
    pow = m.n.n(m);
  ` + s[0])//.replaceAll(/\w+/g, a=>(`(${a})`,a)))
  .replaceAll(/[^\w]\d+/g, (f=>n=>n[0]+f(n.slice(1)))(n=>`f.x.${"f(".repeat(n)}x${")".repeat(n)}`))
  .replaceAll('true', 'x.y.x').replaceAll('false', 'x.y.y')
  .replaceAll('[', '_._(').replaceAll(']', ')')
  .replaceAll('.', '=>')
  .replaceAll(',', ')(')
);