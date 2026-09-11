"""Recover StructuredOutput seeds from a workflow's agent jsonl files (when the
workflow barrier is blocked by one stuck agent). Aggregates all {seeds:[...]}
into a result JSON shaped like the normal workflow output.

Usage: _extract_wf.py <workflow_dir> <out.json>
"""
import os, json, sys, glob

wf_dir, out = sys.argv[1], sys.argv[2]
seeds = []
for f in glob.glob(os.path.join(wf_dir, "*.jsonl")):
    for line in open(f, errors="ignore"):
        line = line.strip()
        if not line or "StructuredOutput" not in line:
            continue
        try:
            obj = json.loads(line)
        except Exception:
            continue
        msg = obj.get("message", {})
        content = msg.get("content", [])
        if not isinstance(content, list):
            continue
        for c in content:
            if isinstance(c, dict) and c.get("type") == "tool_use" and c.get("name") == "StructuredOutput":
                inp = c.get("input", {})
                for s in inp.get("seeds", []):
                    if isinstance(s, dict) and s.get("fn") and s.get("code"):
                        seeds.append(s)

# dedup by fn (keep first)
seen, uniq = set(), []
for s in seeds:
    if s["fn"] in seen:
        continue
    seen.add(s["fn"]); uniq.append(s)

json.dump({"result": {"count": len(uniq), "seeds": uniq}}, open(out, "w"))
print(f"recovered {len(uniq)} seeds -> {out}")
