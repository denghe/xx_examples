var h;
h || (h = typeof Module !== 'undefined' ? Module : {});
var aa = Object.assign({}, h),
  ea = (a, b) => {
    throw b;
  },
  l = "";
"undefined" != typeof document && document.currentScript && (l = document.currentScript.src);
l = l.startsWith("blob:") ? "" : l.substr(0, l.replace(/[?#].*/, "").lastIndexOf("/") + 1);
var fa = h.print || console.log.bind(console),
  p = h.printErr || console.error.bind(console);
Object.assign(h, aa);
aa = null;
h.quit && (ea = h.quit);
var q;
h.wasmBinary && (q = h.wasmBinary);
"object" != typeof WebAssembly && ha("no native wasm support detected");
var ia,
  ja = !1,
  w,
  ka,
  y,
  z,
  la,
  D,
  G,
  ma,
  na;
function oa() {
  var a = ia.buffer;
  h.HEAP8 = ka = new Int8Array(a);
  h.HEAP16 = z = new Int16Array(a);
  h.HEAPU8 = y = new Uint8Array(a);
  h.HEAPU16 = la = new Uint16Array(a);
  h.HEAP32 = D = new Int32Array(a);
  h.HEAPU32 = G = new Uint32Array(a);
  h.HEAPF32 = ma = new Float32Array(a);
  h.HEAPF64 = na = new Float64Array(a);
}
var pa = [],
  qa = [],
  ra = [],
  sa = [];
function ta() {
  var a = h.preRun.shift();
  pa.unshift(a);
}
var I = 0,
  ua = null,
  J = null;
function va() {
  var _h$monitorRunDependen, _h;
  I++;
  (_h$monitorRunDependen = (_h = h).monitorRunDependencies) === null || _h$monitorRunDependen === void 0 || _h$monitorRunDependen.call(_h, I);
}
function wa() {
  var _h$monitorRunDependen2, _h2;
  I--;
  (_h$monitorRunDependen2 = (_h2 = h).monitorRunDependencies) === null || _h$monitorRunDependen2 === void 0 || _h$monitorRunDependen2.call(_h2, I);
  if (0 == I && (null !== ua && (clearInterval(ua), ua = null), J)) {
    var a = J;
    J = null;
    a();
  }
}
function ha(a) {
  var _h$onAbort, _h3;
  (_h$onAbort = (_h3 = h).onAbort) === null || _h$onAbort === void 0 || _h$onAbort.call(_h3, a);
  a = "Aborted(" + a + ")";
  p(a);
  ja = !0;
  w = 1;
  throw new WebAssembly.RuntimeError(a + ". Build with -sASSERTIONS for more info.");
}
var xa = a => a.startsWith("data:application/octet-stream;base64,"),
  K;
K = "9.wasm";
if (!xa(K)) {
  var ya = K;
  K = h.locateFile ? h.locateFile(ya, l) : l + ya;
}
function za(a) {
  if (a == K && q) return new Uint8Array(q);
  throw "both async and sync fetching of the wasm failed";
}
function Aa(a) {
  return q || "function" != typeof fetch ? Promise.resolve().then(() => za(a)) : fetch(a, {
    credentials: "same-origin"
  }).then(b => {
    if (!b.ok) throw `failed to load wasm binary file at '${a}'`;
    return b.arrayBuffer();
  }).catch(() => za(a));
}
function Ba(a, b, c) {
  return Aa(a).then(d => WebAssembly.instantiate(d, b)).then(c, d => {
    p(`failed to asynchronously prepare wasm: ${d}`);
    ha(d);
  });
}
function Ca(a, b) {
  var c = K;
  q || "function" != typeof WebAssembly.instantiateStreaming || xa(c) || "function" != typeof fetch ? Ba(c, a, b) : fetch(c, {
    credentials: "same-origin"
  }).then(d => WebAssembly.instantiateStreaming(d, a).then(b, function (g) {
    p(`wasm streaming compile failed: ${g}`);
    p("falling back to ArrayBuffer instantiation");
    return Ba(c, a, b);
  }));
}
function Da(a) {
  this.name = "ExitStatus";
  this.message = `Program terminated with exit(${a})`;
  this.status = a;
}
var Ea = a => {
    for (; 0 < a.length;) a.shift()(h);
  },
  Fa = h.noExitRuntime || !0,
  Ga = new TextDecoder("utf8"),
  L = (a, b) => {
    if (!a) return "";
    b = a + b;
    for (var c = a; !(c >= b) && y[c];) ++c;
    return Ga.decode(y.subarray(a, c));
  };
class Ha {
  constructor(a) {
    this.ya = a - 24;
  }
}
var Ia = 0,
  Ja = 0,
  O = a => 0 === a % 4 && (0 !== a % 100 || 0 === a % 400),
  Ka = [0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335],
  La = [0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334],
  Ma = a => {
    for (var b = 0, c = 0; c < a.length; ++c) {
      var d = a.charCodeAt(c);
      127 >= d ? b++ : 2047 >= d ? b += 2 : 55296 <= d && 57343 >= d ? (b += 4, ++c) : b += 3;
    }
    return b;
  },
  P = (a, b, c, d) => {
    if (!(0 < d)) return 0;
    var g = c;
    d = c + d - 1;
    for (var k = 0; k < a.length; ++k) {
      var f = a.charCodeAt(k);
      if (55296 <= f && 57343 >= f) {
        var n = a.charCodeAt(++k);
        f = 65536 + ((f & 1023) << 10) | n & 1023;
      }
      if (127 >= f) {
        if (c >= d) break;
        b[c++] = f;
      } else {
        if (2047 >= f) {
          if (c + 1 >= d) break;
          b[c++] = 192 | f >> 6;
        } else {
          if (65535 >= f) {
            if (c + 2 >= d) break;
            b[c++] = 224 | f >> 12;
          } else {
            if (c + 3 >= d) break;
            b[c++] = 240 | f >> 18;
            b[c++] = 128 | f >> 12 & 63;
          }
          b[c++] = 128 | f >> 6 & 63;
        }
        b[c++] = 128 | f & 63;
      }
    }
    b[c] = 0;
    return c - g;
  },
  Na = a => {
    var b = Ma(a) + 1,
      c = Q(b);
    c && P(a, y, c, b);
    return c;
  },
  R,
  Oa = 0,
  Pa = [];
function Qa() {
  if (navigator.userActivation ? navigator.userActivation.isActive : Oa && Ra.Va) for (var a = 0; a < Pa.length; ++a) {
    var b = Pa[a];
    Pa.splice(a, 1);
    --a;
    b.kb(...b.eb);
  }
}
var S = [];
function Sa(a) {
  var b = S[a];
  b.target.removeEventListener(b.Ca, b.Pa, b.Ma);
  S.splice(a, 1);
}
function Ta(a) {
  if (!a.target) return -4;
  if (a.Na) a.Pa = function (c) {
    ++Oa;
    Ra = a;
    Qa();
    a.Qa(c);
    Qa();
    --Oa;
  }, a.target.addEventListener(a.Ca, a.Pa, a.Ma), S.push(a);else for (var b = 0; b < S.length; ++b) S[b].target == a.target && S[b].Ca == a.Ca && Sa(b--);
  return 0;
}
var Ra,
  Ua,
  Va,
  Wa = [0, document, window],
  Xa = a => {
    a = 2 < a ? L(a) : a;
    return Wa[a] || document.querySelector(a);
  },
  Ya = (a, b, c, d, g, k) => {
    Ua || (Ua = Q(176));
    a = {
      target: Xa(a),
      Ca: k,
      Na: d,
      Qa: f => {
        var n = Ua;
        na[n >> 3] = f.timeStamp;
        var m = n >> 2;
        D[m + 2] = f.location;
        D[m + 3] = f.ctrlKey;
        D[m + 4] = f.shiftKey;
        D[m + 5] = f.altKey;
        D[m + 6] = f.metaKey;
        D[m + 7] = f.repeat;
        D[m + 8] = f.charCode;
        D[m + 9] = f.keyCode;
        D[m + 10] = f.which;
        P(f.key || "", y, n + 44, 32);
        P(f.code || "", y, n + 76, 32);
        P(f.char || "", y, n + 108, 32);
        P(f.locale || "", y, n + 140, 32);
        R.get(d)(g, n, b) && f.preventDefault();
      },
      Ma: c
    };
    return Ta(a);
  },
  Za = (a, b, c, d, g, k) => {
    Va || (Va = Q(72));
    a = Xa(a);
    return Ta({
      target: a,
      Va: "mousemove" != k && "mouseenter" != k && "mouseleave" != k,
      Ca: k,
      Na: d,
      Qa: function () {
        let f = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : event;
        var n = a,
          m = Va;
        na[m >> 3] = f.timeStamp;
        m >>= 2;
        D[m + 2] = f.screenX;
        D[m + 3] = f.screenY;
        D[m + 4] = f.clientX;
        D[m + 5] = f.clientY;
        D[m + 6] = f.ctrlKey;
        D[m + 7] = f.shiftKey;
        D[m + 8] = f.altKey;
        D[m + 9] = f.metaKey;
        z[2 * m + 20] = f.button;
        z[2 * m + 21] = f.buttons;
        D[m + 11] = f.movementX;
        D[m + 12] = f.movementY;
        n = 0 > Wa.indexOf(n) ? n.getBoundingClientRect() : {
          left: 0,
          top: 0
        };
        D[m + 13] = f.clientX - n.left;
        D[m + 14] = f.clientY - n.top;
        R.get(d)(g, Va, b) && f.preventDefault();
      },
      Ma: c
    });
  };
function $a(a) {
  var b = T,
    c = b.Ta.pop() || b.ya.length;
  b.ya[c] = a;
  return c;
}
class ab {
  constructor() {
    this.ya = [void 0];
    this.Ta = [];
  }
  get(a) {
    return this.ya[a];
  }
  has(a) {
    return void 0 !== this.ya[a];
  }
}
var T, bb;
function cb(a, b, c, d, g) {
  function k() {
    var A = 0,
      F = 0;
    r.response && H && 0 === G[a + 12 >> 2] && (F = r.response.byteLength);
    0 < F && (A = Q(F), y.set(new Uint8Array(r.response), A));
    G[a + 12 >> 2] = A;
    U(a + 16, F);
    U(a + 24, 0);
    (A = r.response ? r.response.byteLength : 0) && U(a + 32, A);
    z[a + 40 >> 1] = r.readyState;
    z[a + 42 >> 1] = r.status;
    r.statusText && P(r.statusText, y, a + 44, 64);
  }
  var f = G[a + 8 >> 2];
  if (f) {
    var n = L(f),
      m = a + 112,
      t = L(m + 0);
    t || (t = "GET");
    var B = G[m + 56 >> 2],
      E = G[m + 68 >> 2],
      M = G[m + 72 >> 2];
    f = G[m + 76 >> 2];
    var e = G[m + 80 >> 2],
      u = G[m + 84 >> 2],
      v = G[m + 88 >> 2],
      C = G[m + 52 >> 2],
      H = !!(C & 1),
      ba = !!(C & 2);
    C = !!(C & 64);
    E = E ? L(E) : void 0;
    M = M ? L(M) : void 0;
    var r = new XMLHttpRequest();
    r.withCredentials = !!y[m + 60];
    r.open(t, n, !C, E, M);
    C || (r.timeout = B);
    r.ya = n;
    r.responseType = "arraybuffer";
    e && (n = L(e), r.overrideMimeType(n));
    if (f) for (;;) {
      m = G[f >> 2];
      if (!m) break;
      n = G[f + 4 >> 2];
      if (!n) break;
      f += 8;
      m = L(m);
      n = L(n);
      r.setRequestHeader(m, n);
    }
    var N = $a(r);
    G[a >> 2] = N;
    f = u && v ? y.slice(u, u + v) : null;
    r.onload = A => {
      T.has(N) && (k(), 200 <= r.status && 300 > r.status ? b === null || b === void 0 ? void 0 : b(a, r, A) : c === null || c === void 0 ? void 0 : c(a, r, A));
    };
    r.onerror = A => {
      T.has(N) && (k(), c === null || c === void 0 ? void 0 : c(a, r, A));
    };
    r.ontimeout = A => {
      T.has(N) && (c === null || c === void 0 ? void 0 : c(a, r, A));
    };
    r.onprogress = A => {
      if (T.has(N)) {
        var F = H && ba && r.response ? r.response.byteLength : 0,
          x = 0;
        0 < F && H && ba && (x = Q(F), y.set(new Uint8Array(r.response), x));
        G[a + 12 >> 2] = x;
        U(a + 16, F);
        U(a + 24, A.loaded - F);
        U(a + 32, A.total);
        z[a + 40 >> 1] = r.readyState;
        3 <= r.readyState && 0 === r.status && 0 < A.loaded && (r.status = 200);
        z[a + 42 >> 1] = r.status;
        r.statusText && P(r.statusText, y, a + 44, 64);
        d === null || d === void 0 || d(a, r, A);
        x && db(x);
      }
    };
    r.onreadystatechange = A => {
      T.has(N) && (z[a + 40 >> 1] = r.readyState, 2 <= r.readyState && (z[a + 42 >> 1] = r.status), g === null || g === void 0 ? void 0 : g(a, r, A));
    };
    try {
      r.send(f);
    } catch (A) {
      c === null || c === void 0 || c(a, r, A);
    }
  } else c(a, 0, "no url specified!");
}
var eb = a => {
    a instanceof Da || "unwind" == a || ea(1, a);
  },
  fb = a => {
    var _h$onExit, _h4;
    w = a;
    Fa || ((_h$onExit = (_h4 = h).onExit) !== null && _h$onExit !== void 0 && _h$onExit.call(_h4, a), ja = !0);
    ea(a, new Da(a));
  },
  U = (a, b) => {
    G[a >> 2] = b;
    G[a + 4 >> 2] = (b - G[a >> 2]) / 4294967296;
  };
function gb(a, b, c, d) {
  var g = bb;
  if (g) {
    var k = G[a + 112 + 64 >> 2];
    k || (k = G[a + 8 >> 2]);
    var f = L(k);
    try {
      var n = g.transaction(["FILES"], "readwrite").objectStore("FILES").put(b, f);
      n.onsuccess = () => {
        z[a + 40 >> 1] = 4;
        z[a + 42 >> 1] = 200;
        P("OK", y, a + 44, 64);
        c(a, 0, f);
      };
      n.onerror = m => {
        z[a + 40 >> 1] = 4;
        z[a + 42 >> 1] = 413;
        P("Payload Too Large", y, a + 44, 64);
        d(a, 0, m);
      };
    } catch (m) {
      d(a, 0, m);
    }
  } else d(a, 0, "IndexedDB not available!");
}
function hb(a, b, c) {
  var d = bb;
  if (d) {
    var g = G[a + 112 + 64 >> 2];
    g || (g = G[a + 8 >> 2]);
    g = L(g);
    try {
      var k = d.transaction(["FILES"], "readonly").objectStore("FILES").get(g);
      k.onsuccess = f => {
        if (f.target.result) {
          f = f.target.result;
          var n = f.byteLength || f.length,
            m = Q(n);
          y.set(new Uint8Array(f), m);
          G[a + 12 >> 2] = m;
          U(a + 16, n);
          U(a + 24, 0);
          U(a + 32, n);
          z[a + 40 >> 1] = 4;
          z[a + 42 >> 1] = 200;
          P("OK", y, a + 44, 64);
          b(a, 0, f);
        } else z[a + 40 >> 1] = 4, z[a + 42 >> 1] = 404, P("Not Found", y, a + 44, 64), c(a, 0, "no data");
      };
      k.onerror = f => {
        z[a + 40 >> 1] = 4;
        z[a + 42 >> 1] = 404;
        P("Not Found", y, a + 44, 64);
        c(a, 0, f);
      };
    } catch (f) {
      c(a, 0, f);
    }
  } else c(a, 0, "IndexedDB not available!");
}
function ib(a, b, c) {
  var d = bb;
  if (d) {
    var g = G[a + 112 + 64 >> 2];
    g || (g = G[a + 8 >> 2]);
    g = L(g);
    try {
      var k = d.transaction(["FILES"], "readwrite").objectStore("FILES").delete(g);
      k.onsuccess = f => {
        f = f.target.result;
        G[a + 12 >> 2] = 0;
        U(a + 16, 0);
        U(a + 24, 0);
        U(a + 32, 0);
        z[a + 40 >> 1] = 4;
        z[a + 42 >> 1] = 200;
        P("OK", y, a + 44, 64);
        b(a, 0, f);
      };
      k.onerror = f => {
        z[a + 40 >> 1] = 4;
        z[a + 42 >> 1] = 404;
        P("Not Found", y, a + 44, 64);
        c(a, 0, f);
      };
    } catch (f) {
      c(a, 0, f);
    }
  } else c(a, 0, "IndexedDB not available!");
}
var jb = 1,
  kb = [],
  V = [],
  lb = [],
  W = [],
  X = [],
  mb = [],
  nb = [],
  ob = a => {
    for (var b = jb++, c = a.length; c < b; c++) a[c] = null;
    return b;
  },
  pb = (a, b, c, d) => {
    for (var g = 0; g < a; g++) {
      var k = Y[c](),
        f = k && ob(d);
      k && (k.name = f, d[f] = k);
      D[b + 4 * g >> 2] = f;
    }
  },
  rb = (a, b) => {
    a.ya || (a.ya = a.getContext, a.getContext = function (d, g) {
      g = a.ya(d, g);
      return "webgl" == d == g instanceof WebGLRenderingContext ? g : null;
    });
    var c = a.getContext("webgl2", b);
    return c ? qb(c, b) : 0;
  },
  qb = (a, b) => {
    var c = ob(nb);
    b = {
      handle: c,
      attributes: b,
      version: b.Ya,
      Ua: a
    };
    a.canvas && (a.canvas.cb = b);
    nb[c] = b;
    return c;
  },
  sb,
  tb = ["default", "low-power", "high-performance"],
  ub = [null, [], []],
  vb = a => "]" == a.slice(-1) && a.lastIndexOf("["),
  wb = a => {
    var b = Y.Wa,
      c = b.Da[a];
    "number" == typeof c && (b.Da[a] = c = Y.getUniformLocation(b, b.Ra[a] + (0 < c ? `[${c}]` : "")));
    return c;
  },
  xb = [31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31],
  yb = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
function zb(a) {
  var b = Array(Ma(a) + 1);
  P(a, b, 0, b.length);
  return b;
}
T = new ab();
va();
(function (a, b) {
  try {
    var c = indexedDB.open("emscripten_filesystem", 1);
  } catch (d) {
    b(d);
    return;
  }
  c.onupgradeneeded = d => {
    d = d.target.result;
    d.objectStoreNames.contains("FILES") && d.deleteObjectStore("FILES");
    d.createObjectStore("FILES");
  };
  c.onsuccess = d => a(d.target.result);
  c.onerror = b;
})(a => {
  bb = a;
  wa();
}, () => {
  bb = !1;
  wa();
});
var Y,
  Ab = {
    e: (a, b, c, d) => {
      ha(`Assertion failed: ${L(a)}, at: ` + [b ? L(b) : "unknown filename", c, d ? L(d) : "unknown function"]);
    },
    t: (a, b, c) => {
      var d = new Ha(a);
      G[d.ya + 16 >> 2] = 0;
      G[d.ya + 4 >> 2] = b;
      G[d.ya + 8 >> 2] = c;
      Ia = a;
      Ja++;
      throw Ia;
    },
    U: function (a) {
      if (T.has(a)) {
        var b = T.get(a),
          c = T;
        c.ya[a] = void 0;
        c.Ta.push(a);
        0 < b.readyState && 4 > b.readyState && b.abort();
      }
    },
    R: () => 1,
    N: function (a, b, c) {
      a = new Date(1E3 * (b + 2097152 >>> 0 < 4194305 - !!a ? (a >>> 0) + 4294967296 * b : NaN));
      D[c >> 2] = a.getSeconds();
      D[c + 4 >> 2] = a.getMinutes();
      D[c + 8 >> 2] = a.getHours();
      D[c + 12 >> 2] = a.getDate();
      D[c + 16 >> 2] = a.getMonth();
      D[c + 20 >> 2] = a.getFullYear() - 1900;
      D[c + 24 >> 2] = a.getDay();
      D[c + 28 >> 2] = (O(a.getFullYear()) ? Ka : La)[a.getMonth()] + a.getDate() - 1 | 0;
      D[c + 36 >> 2] = -(60 * a.getTimezoneOffset());
      b = new Date(a.getFullYear(), 6, 1).getTimezoneOffset();
      var d = new Date(a.getFullYear(), 0, 1).getTimezoneOffset();
      D[c + 32 >> 2] = (b != d && a.getTimezoneOffset() == Math.min(d, b)) | 0;
    },
    Q: (a, b, c) => {
      function d(m) {
        return (m = m.toTimeString().match(/\(([A-Za-z ]+)\)$/)) ? m[1] : "GMT";
      }
      var g = new Date().getFullYear(),
        k = new Date(g, 0, 1),
        f = new Date(g, 6, 1);
      g = k.getTimezoneOffset();
      var n = f.getTimezoneOffset();
      G[a >> 2] = 60 * Math.max(g, n);
      D[b >> 2] = Number(g != n);
      a = d(k);
      b = d(f);
      a = Na(a);
      b = Na(b);
      n < g ? (G[c >> 2] = a, G[c + 4 >> 2] = b) : (G[c >> 2] = b, G[c + 4 >> 2] = a);
    },
    r: () => {
      ha("");
    },
    S: () => Date.now(),
    T: () => performance.now(),
    W: () => !0,
    da: (a, b) => {
      function c(d) {
        R.get(a)(d, b) && requestAnimationFrame(c);
      }
      return requestAnimationFrame(c);
    },
    P: a => {
      var b = y.length;
      a >>>= 0;
      if (2147483648 < a) return !1;
      for (var c = 1; 4 >= c; c *= 2) {
        var d = b * (1 + .2 / c);
        d = Math.min(d, a + 100663296);
        var g = Math;
        d = Math.max(a, d);
        a: {
          g = (g.min.call(g, 2147483648, d + (65536 - d % 65536) % 65536) - ia.buffer.byteLength + 65535) / 65536;
          try {
            ia.grow(g);
            oa();
            var k = 1;
            break a;
          } catch (f) {}
          k = void 0;
        }
        if (k) return !0;
      }
      return !1;
    },
    pa: (a, b, c) => {
      a = Xa(a);
      if (!a) return -4;
      a.width = b;
      a.height = c;
      return 0;
    },
    Y: (a, b, c, d) => Ya(a, b, c, d, 2, "keydown"),
    O: (a, b, c, d) => Ya(a, b, c, d, 3, "keyup"),
    M: (a, b, c, d) => Za(a, b, c, d, 5, "mousedown"),
    qa: (a, b, c, d) => Za(a, b, c, d, 8, "mousemove"),
    L: (a, b, c, d) => Za(a, b, c, d, 6, "mouseup"),
    V: function (a, b, c, d, g) {
      function k(x) {
        if (M) x();else if (!ja) try {
          if (x(), !Fa) try {
            w = x = w, fb(x);
          } catch (ca) {
            eb(ca);
          }
        } catch (ca) {
          eb(ca);
        }
      }
      var f = a + 112,
        n = G[f + 36 >> 2],
        m = G[f + 40 >> 2],
        t = G[f + 44 >> 2],
        B = G[f + 48 >> 2],
        E = G[f + 52 >> 2],
        M = !!(E & 64),
        e = x => {
          k(() => {
            n ? R.get(n)(x) : b === null || b === void 0 ? void 0 : b(x);
          });
        },
        u = x => {
          k(() => {
            t ? R.get(t)(x) : d === null || d === void 0 ? void 0 : d(x);
          });
        },
        v = x => {
          k(() => {
            m ? R.get(m)(x) : c === null || c === void 0 ? void 0 : c(x);
          });
        },
        C = x => {
          k(() => {
            B ? R.get(B)(x) : g === null || g === void 0 ? void 0 : g(x);
          });
        },
        H = x => {
          cb(x, e, v, u, C);
        },
        ba = (x, ca) => {
          gb(x, ca.response, da => {
            k(() => {
              n ? R.get(n)(da) : b === null || b === void 0 ? void 0 : b(da);
            });
          }, da => {
            k(() => {
              n ? R.get(n)(da) : b === null || b === void 0 ? void 0 : b(da);
            });
          });
        },
        r = x => {
          cb(x, ba, v, u, C);
        },
        N = L(f + 0),
        A = !!(E & 16),
        F = !!(E & 4);
      E = !!(E & 32);
      if ("EM_IDB_STORE" === N) H = G[f + 84 >> 2], gb(a, y.slice(H, H + G[f + 88 >> 2]), e, v);else if ("EM_IDB_DELETE" === N) ib(a, e, v);else if (A) {
        if (E) return 0;
        cb(a, F ? ba : e, v, u, C);
      } else hb(a, e, E ? v : F ? r : H);
      return a;
    },
    oa: (a, b) => {
      b >>= 2;
      b = {
        alpha: !!D[b],
        depth: !!D[b + 1],
        stencil: !!D[b + 2],
        antialias: !!D[b + 3],
        premultipliedAlpha: !!D[b + 4],
        preserveDrawingBuffer: !!D[b + 5],
        powerPreference: tb[D[b + 6]],
        failIfMajorPerformanceCaveat: !!D[b + 7],
        Ya: D[b + 8],
        hb: D[b + 9],
        gb: D[b + 10],
        Xa: D[b + 11],
        ib: D[b + 12],
        jb: D[b + 13]
      };
      a = Xa(a);
      return !a || b.Xa ? 0 : rb(a, b);
    },
    na: a => {
      var _sb;
      sb = nb[a];
      h.fb = Y = (_sb = sb) === null || _sb === void 0 ? void 0 : _sb.Ua;
      return !a || Y ? 0 : -5;
    },
    v: (a, b, c, d) => {
      for (var g = 0, k = 0; k < c; k++) {
        var f = G[b >> 2],
          n = G[b + 4 >> 2];
        b += 8;
        for (var m = 0; m < n; m++) {
          var t = y[f + m],
            B = ub[a];
          if (0 === t || 10 === t) {
            for (t = 0; B[t] && !(NaN <= t);) ++t;
            t = Ga.decode(B.buffer ? B.subarray(0, t) : new Uint8Array(B.slice(0, t)));
            (1 === a ? fa : p)(t);
            B.length = 0;
          } else B.push(t);
        }
        g += n;
      }
      G[d >> 2] = g;
      return 0;
    },
    la: a => Y.activeTexture(a),
    A: (a, b) => {
      Y.attachShader(V[a], X[b]);
    },
    b: (a, b) => {
      35051 == a ? Y.Oa = b : 35052 == a && (Y.Ja = b);
      Y.bindBuffer(a, kb[b]);
    },
    w: (a, b) => {
      Y.bindFramebuffer(a, lb[b]);
    },
    c: (a, b) => {
      Y.bindTexture(a, W[b]);
    },
    j: a => {
      Y.bindVertexArray(mb[a]);
    },
    o: a => Y.blendEquation(a),
    p: (a, b) => Y.blendFunc(a, b),
    i: (a, b, c, d) => {
      c && b ? Y.bufferData(a, y, d, c, b) : Y.bufferData(a, b, d);
    },
    D: a => Y.clear(a),
    E: (a, b, c, d) => Y.clearColor(a, b, c, d),
    ea: a => {
      Y.compileShader(X[a]);
    },
    ja: () => {
      var a = ob(V),
        b = Y.createProgram();
      b.name = a;
      b.Ga = b.Ea = b.Fa = 0;
      b.La = 1;
      V[a] = b;
      return a;
    },
    ga: a => {
      var b = ob(X);
      X[b] = Y.createShader(a);
      return b;
    },
    q: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = D[b + 4 * c >> 2],
          g = kb[d];
        g && (Y.deleteBuffer(g), g.name = 0, kb[d] = null, d == Y.Oa && (Y.Oa = 0), d == Y.Ja && (Y.Ja = 0));
      }
    },
    F: (a, b) => {
      for (var c = 0; c < a; ++c) {
        var d = D[b + 4 * c >> 2],
          g = lb[d];
        g && (Y.deleteFramebuffer(g), g.name = 0, lb[d] = null);
      }
    },
    k: a => {
      if (a) {
        var b = V[a];
        b && (Y.deleteProgram(b), b.name = 0, V[a] = null);
      }
    },
    d: a => {
      if (a) {
        var b = X[a];
        b && (Y.deleteShader(b), X[a] = null);
      }
    },
    _: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = D[b + 4 * c >> 2],
          g = W[d];
        g && (Y.deleteTexture(g), g.name = 0, W[d] = null);
      }
    },
    J: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = D[b + 4 * c >> 2];
        Y.deleteVertexArray(mb[d]);
        mb[d] = null;
      }
    },
    K: a => Y.disable(a),
    B: (a, b, c, d) => {
      Y.drawArraysInstanced(a, b, c, d);
    },
    G: (a, b, c, d) => {
      Y.drawElements(a, b, c, d);
    },
    ma: a => Y.enable(a),
    f: a => {
      Y.enableVertexAttribArray(a);
    },
    Z: (a, b, c, d, g) => {
      Y.framebufferTexture2D(a, b, c, W[d], g);
    },
    n: (a, b) => {
      pb(a, b, "createBuffer", kb);
    },
    ba: (a, b) => {
      pb(a, b, "createFramebuffer", lb);
    },
    l: (a, b) => {
      pb(a, b, "createTexture", W);
    },
    C: (a, b) => {
      pb(a, b, "createVertexArray", mb);
    },
    h: (a, b) => Y.getAttribLocation(V[a], L(b)),
    ha: (a, b, c, d) => {
      a = Y.getProgramInfoLog(V[a]);
      b = 0 < b && d ? P(a, y, d, b) : 0;
      c && (D[c >> 2] = b);
    },
    z: (a, b, c) => {
      if (c && !(a >= jb)) if (a = V[a], 35716 == b) D[c >> 2] = Y.getProgramInfoLog(a).length + 1;else if (35719 == b) {
        if (!a.Ga) for (b = 0; b < Y.getProgramParameter(a, 35718); ++b) a.Ga = Math.max(a.Ga, Y.getActiveUniform(a, b).name.length + 1);
        D[c >> 2] = a.Ga;
      } else if (35722 == b) {
        if (!a.Ea) for (b = 0; b < Y.getProgramParameter(a, 35721); ++b) a.Ea = Math.max(a.Ea, Y.getActiveAttrib(a, b).name.length + 1);
        D[c >> 2] = a.Ea;
      } else if (35381 == b) {
        if (!a.Fa) for (b = 0; b < Y.getProgramParameter(a, 35382); ++b) a.Fa = Math.max(a.Fa, Y.getActiveUniformBlockName(a, b).length + 1);
        D[c >> 2] = a.Fa;
      } else D[c >> 2] = Y.getProgramParameter(a, b);
    },
    ca: (a, b, c, d) => {
      a = Y.getShaderInfoLog(X[a]);
      b = 0 < b && d ? P(a, y, d, b) : 0;
      c && (D[c >> 2] = b);
    },
    y: (a, b, c) => {
      c && (35716 == b ? (a = Y.getShaderInfoLog(X[a]), D[c >> 2] = a ? a.length + 1 : 0) : 35720 == b ? (a = Y.getShaderSource(X[a]), D[c >> 2] = a ? a.length + 1 : 0) : D[c >> 2] = Y.getShaderParameter(X[a], b));
    },
    s: (a, b) => {
      b = L(b);
      if (a = V[a]) {
        var c = a,
          d = c.Da,
          g = c.Sa,
          k;
        if (!d) for (c.Da = d = {}, c.Ra = {}, k = 0; k < Y.getProgramParameter(c, 35718); ++k) {
          var f = Y.getActiveUniform(c, k);
          var n = f.name;
          f = f.size;
          var m = vb(n);
          m = 0 < m ? n.slice(0, m) : n;
          var t = c.La;
          c.La += f;
          g[m] = [f, t];
          for (n = 0; n < f; ++n) d[t] = n, c.Ra[t++] = m;
        }
        c = a.Da;
        d = 0;
        g = b;
        k = vb(b);
        0 < k && (d = parseInt(b.slice(k + 1)) >>> 0, g = b.slice(0, k));
        if ((g = a.Sa[g]) && d < g[0] && (d += g[1], c[d] = c[d] || Y.getUniformLocation(a, b))) return d;
      }
      return -1;
    },
    ia: a => {
      a = V[a];
      Y.linkProgram(a);
      a.Da = 0;
      a.Sa = {};
    },
    fa: (a, b, c, d) => {
      for (var g = "", k = 0; k < b; ++k) g += L(G[c + 4 * k >> 2], d ? G[d + 4 * k >> 2] : void 0);
      Y.shaderSource(X[a], g);
    },
    x: (a, b, c, d, g, k, f, n, m) => {
      if (Y.Ja) Y.texImage2D(a, b, c, d, g, k, f, n, m);else if (m) {
        var t = n - 5120;
        t = 0 == t ? ka : 1 == t ? y : 2 == t ? z : 4 == t ? D : 6 == t ? ma : 5 == t || 28922 == t || 28520 == t || 30779 == t || 30782 == t ? G : la;
        Y.texImage2D(a, b, c, d, g, k, f, n, t, m >>> 31 - Math.clz32(t.BYTES_PER_ELEMENT));
      } else Y.texImage2D(a, b, c, d, g, k, f, n, null);
    },
    a: (a, b, c) => Y.texParameteri(a, b, c),
    ka: (a, b) => {
      Y.uniform1i(wb(a), b);
    },
    H: (a, b, c) => {
      Y.uniform2f(wb(a), b, c);
    },
    I: a => {
      a = V[a];
      Y.useProgram(a);
      Y.Wa = a;
    },
    m: (a, b) => {
      Y.vertexAttribDivisor(a, b);
    },
    g: (a, b, c, d, g, k) => {
      Y.vertexAttribPointer(a, b, c, !!d, g, k);
    },
    u: (a, b, c, d) => Y.viewport(a, b, c, d),
    aa: function (a, b, c, d) {
      var g = document.createElement("canvas");
      g.width = b;
      g.height = c;
      b = g.getContext("2d");
      b.font = a + "px " + L(d);
      b.textBaseline = "middle";
      b.globalAlpha = 1;
      b.fillStyle = "white";
      window["gCanvas" + a] = g;
      window["gCanvasCtx" + a] = b;
    },
    X: function (a, b, c, d) {
      var g = new Image();
      g.onload = () => {
        G[c >> 2] = g.width;
        G[d >> 2] = g.height;
        Y.bindTexture(Y.TEXTURE_2D, W[a]);
        Y.texImage2D(Y.TEXTURE_2D, 0, Y.RGBA, Y.RGBA, Y.UNSIGNED_BYTE, g);
      };
      g.src = L(b);
    },
    ra: (a, b, c, d) => {
      function g(e, u, v) {
        for (e = "number" == typeof e ? e.toString() : e || ""; e.length < u;) e = v[0] + e;
        return e;
      }
      function k(e, u) {
        return g(e, u, "0");
      }
      function f(e, u) {
        function v(H) {
          return 0 > H ? -1 : 0 < H ? 1 : 0;
        }
        var C;
        0 === (C = v(e.getFullYear() - u.getFullYear())) && 0 === (C = v(e.getMonth() - u.getMonth())) && (C = v(e.getDate() - u.getDate()));
        return C;
      }
      function n(e) {
        switch (e.getDay()) {
          case 0:
            return new Date(e.getFullYear() - 1, 11, 29);
          case 1:
            return e;
          case 2:
            return new Date(e.getFullYear(), 0, 3);
          case 3:
            return new Date(e.getFullYear(), 0, 2);
          case 4:
            return new Date(e.getFullYear(), 0, 1);
          case 5:
            return new Date(e.getFullYear() - 1, 11, 31);
          case 6:
            return new Date(e.getFullYear() - 1, 11, 30);
        }
      }
      function m(e) {
        var u = e.Aa;
        for (e = new Date(new Date(e.Ba + 1900, 0, 1).getTime()); 0 < u;) {
          var v = e.getMonth(),
            C = (O(e.getFullYear()) ? xb : yb)[v];
          if (u > C - e.getDate()) u -= C - e.getDate() + 1, e.setDate(1), 11 > v ? e.setMonth(v + 1) : (e.setMonth(0), e.setFullYear(e.getFullYear() + 1));else {
            e.setDate(e.getDate() + u);
            break;
          }
        }
        v = new Date(e.getFullYear() + 1, 0, 4);
        u = n(new Date(e.getFullYear(), 0, 4));
        v = n(v);
        return 0 >= f(u, e) ? 0 >= f(v, e) ? e.getFullYear() + 1 : e.getFullYear() : e.getFullYear() - 1;
      }
      var t = G[d + 40 >> 2];
      d = {
        ab: D[d >> 2],
        $a: D[d + 4 >> 2],
        Ha: D[d + 8 >> 2],
        Ka: D[d + 12 >> 2],
        Ia: D[d + 16 >> 2],
        Ba: D[d + 20 >> 2],
        za: D[d + 24 >> 2],
        Aa: D[d + 28 >> 2],
        lb: D[d + 32 >> 2],
        Za: D[d + 36 >> 2],
        bb: t ? L(t) : ""
      };
      c = L(c);
      t = {
        "%c": "%a %b %d %H:%M:%S %Y",
        "%D": "%m/%d/%y",
        "%F": "%Y-%m-%d",
        "%h": "%b",
        "%r": "%I:%M:%S %p",
        "%R": "%H:%M",
        "%T": "%H:%M:%S",
        "%x": "%m/%d/%y",
        "%X": "%H:%M:%S",
        "%Ec": "%c",
        "%EC": "%C",
        "%Ex": "%m/%d/%y",
        "%EX": "%H:%M:%S",
        "%Ey": "%y",
        "%EY": "%Y",
        "%Od": "%d",
        "%Oe": "%e",
        "%OH": "%H",
        "%OI": "%I",
        "%Om": "%m",
        "%OM": "%M",
        "%OS": "%S",
        "%Ou": "%u",
        "%OU": "%U",
        "%OV": "%V",
        "%Ow": "%w",
        "%OW": "%W",
        "%Oy": "%y"
      };
      for (var B in t) c = c.replace(new RegExp(B, "g"), t[B]);
      var E = "Sunday Monday Tuesday Wednesday Thursday Friday Saturday".split(" "),
        M = "January February March April May June July August September October November December".split(" ");
      t = {
        "%a": e => E[e.za].substring(0, 3),
        "%A": e => E[e.za],
        "%b": e => M[e.Ia].substring(0, 3),
        "%B": e => M[e.Ia],
        "%C": e => k((e.Ba + 1900) / 100 | 0, 2),
        "%d": e => k(e.Ka, 2),
        "%e": e => g(e.Ka, 2, " "),
        "%g": e => m(e).toString().substring(2),
        "%G": m,
        "%H": e => k(e.Ha, 2),
        "%I": e => {
          e = e.Ha;
          0 == e ? e = 12 : 12 < e && (e -= 12);
          return k(e, 2);
        },
        "%j": e => {
          for (var u = 0, v = 0; v <= e.Ia - 1; u += (O(e.Ba + 1900) ? xb : yb)[v++]);
          return k(e.Ka + u, 3);
        },
        "%m": e => k(e.Ia + 1, 2),
        "%M": e => k(e.$a, 2),
        "%n": () => "\n",
        "%p": e => 0 <= e.Ha && 12 > e.Ha ? "AM" : "PM",
        "%S": e => k(e.ab, 2),
        "%t": () => "\t",
        "%u": e => e.za || 7,
        "%U": e => k(Math.floor((e.Aa + 7 - e.za) / 7), 2),
        "%V": e => {
          var u = Math.floor((e.Aa + 7 - (e.za + 6) % 7) / 7);
          2 >= (e.za + 371 - e.Aa - 2) % 7 && u++;
          if (u) 53 == u && (v = (e.za + 371 - e.Aa) % 7, 4 == v || 3 == v && O(e.Ba) || (u = 1));else {
            u = 52;
            var v = (e.za + 7 - e.Aa - 1) % 7;
            (4 == v || 5 == v && O(e.Ba % 400 - 1)) && u++;
          }
          return k(u, 2);
        },
        "%w": e => e.za,
        "%W": e => k(Math.floor((e.Aa + 7 - (e.za + 6) % 7) / 7), 2),
        "%y": e => (e.Ba + 1900).toString().substring(2),
        "%Y": e => e.Ba + 1900,
        "%z": e => {
          e = e.Za;
          var u = 0 <= e;
          e = Math.abs(e) / 60;
          return (u ? "+" : "-") + String("0000" + (e / 60 * 100 + e % 60)).slice(-4);
        },
        "%Z": e => e.bb,
        "%%": () => "%"
      };
      c = c.replace(/%%/g, "\x00\x00");
      for (B in t) c.includes(B) && (c = c.replace(new RegExp(B, "g"), t[B](d)));
      c = c.replace(/\0\0/g, "%");
      B = zb(c);
      if (B.length > b) return 0;
      ka.set(B, a);
      return B.length - 1;
    },
    $: function (a, b) {
      var c = window["gCanvas" + a];
      a = window["gCanvasCtx" + a];
      a.clearRect(0, 0, c.width, c.height);
      b = L(b);
      var d = a.measureText(b).width;
      a.fillText(b, 0, c.height / 2);
      Y.texImage2D(Y.TEXTURE_2D, 0, Y.RGBA, Y.RGBA, Y.UNSIGNED_BYTE, c);
      return d;
    }
  },
  Z = function () {
    function a(c) {
      Z = c.exports;
      ia = Z.sa;
      oa();
      R = Z.wa;
      qa.unshift(Z.ta);
      wa();
      return Z;
    }
    var b = {
      a: Ab
    };
    va();
    if (h.instantiateWasm) try {
      return h.instantiateWasm(b, a);
    } catch (c) {
      return p(`Module.instantiateWasm callback failed with error: ${c}`), !1;
    }
    Ca(b, function (c) {
      a(c.instance);
    });
    return {};
  }(),
  db = a => (db = Z.ua)(a),
  Q = a => (Q = Z.va)(a),
  Bb = h._main = (a, b) => (Bb = h._main = Z.xa)(a, b),
  Cb;
J = function Db() {
  Cb || Eb();
  Cb || (J = Db);
};
function Eb() {
  function a() {
    if (!Cb && (Cb = !0, h.calledRun = !0, !ja)) {
      Ea(qa);
      Ea(ra);
      if (h.onRuntimeInitialized) h.onRuntimeInitialized();
      if (Fb) {
        var b = Bb;
        try {
          var c = b(0, 0);
          w = c;
          fb(c);
        } catch (d) {
          eb(d);
        }
      }
      if (h.postRun) for ("function" == typeof h.postRun && (h.postRun = [h.postRun]); h.postRun.length;) b = h.postRun.shift(), sa.unshift(b);
      Ea(sa);
    }
  }
  if (!(0 < I)) {
    if (h.preRun) for ("function" == typeof h.preRun && (h.preRun = [h.preRun]); h.preRun.length;) ta();
    Ea(pa);
    0 < I || (h.setStatus ? (h.setStatus("Running..."), setTimeout(function () {
      setTimeout(function () {
        h.setStatus("");
      }, 1);
      a();
    }, 1)) : a());
  }
}
if (h.preInit) for ("function" == typeof h.preInit && (h.preInit = [h.preInit]); 0 < h.preInit.length;) h.preInit.pop()();
var Fb = !0;
h.noInitialRun && (Fb = !1);
Eb();
