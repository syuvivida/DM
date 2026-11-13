"""Evaluate the Pseudoscalar_2HDMI UFO: internal parameters and the analytic 2-body widths of decays.py."""
import cmath, math, re, sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "Pseudoscalar_2HDMI"))
import parameters as P
import decays as D

def evaluate(inputs):
    ns = {"cmath": cmath, "math": math}
    for p in P.all_parameters:
        if p.nature == "external":
            ns[p.name] = inputs.get(p.name, p.value)
    for p in P.all_parameters:
        if p.nature == "internal":
            ns[p.name] = eval(p.value, ns)
    return ns

def widths(inputs, particle):
    ns = evaluate(inputs)
    dec = [d for d in D.all_decays if d.particle.name == particle][0]
    m0 = ns[dec.particle.mass.name].real
    out = {}
    for parts, expr in dec.partial_widths.items():
        ms = [ns[p.mass.name].real if p.mass.name != "ZERO" else 0. for p in parts]
        if m0 <= sum(ms) + 1e-9:
            continue
        out[tuple(p.name for p in parts)] = eval(expr, ns).real
    return out
