"""Tracking replay and impairment injector.

This script reads detection records (JSON Lines) with at least the following fields:
  ts_ms, id, x, y, w, h, score

It can apply:
 - fixed lag (add constant ms to timestamps)
 - jitter + dropout (add zero-mean Gaussian noise to timestamps and randomly drop frames)

The output is a JSON Lines file with adjusted timestamps that can be fed to the tracker.
Optionally, it will invoke an external tracker command after generating the impaired file.

Example input line (JSONL):
{"ts_ms": 1610000000000, "id": -1, "x":0.1, "y":0.1, "w":0.02, "h":0.03, "score":0.85}
"""

import argparse
import json
import random
import math
import subprocess
import sys
import os


def read_jsonl(path):
	with open(path, 'r', encoding='utf-8') as f:
		for line in f:
			line = line.strip()
			if not line:
				continue
			yield json.loads(line)


def write_jsonl(path, items):
	with open(path, 'w', encoding='utf-8') as f:
		for it in items:
			f.write(json.dumps(it, ensure_ascii=False) + '\n')


def apply_fixed_lag(items, lag_ms):
	out = []
	for it in items:
		nit = dict(it)
		nit['ts_ms'] = int(nit.get('ts_ms', 0) + lag_ms)
		out.append(nit)
	return out


def apply_jitter_and_drop(items, sigma_ms, drop_p):
	out = []
	for it in items:
		if random.random() < drop_p:
			continue
		noise = random.gauss(0.0, sigma_ms)
		nit = dict(it)
		nit['ts_ms'] = int(nit.get('ts_ms', 0) + noise)
		out.append(nit)
	return out


def main():
	p = argparse.ArgumentParser()
	p.add_argument('--input', required=True, help='Input detections JSONL')
	p.add_argument('--output', required=True, help='Output impaired JSONL')
	p.add_argument('--mode', choices=['lag', 'jitter'], default='lag')
	p.add_argument('--lag-ms', type=float, default=50.0, help='Fixed lag in ms')
	p.add_argument('--sigma-ms', type=float, default=20.0, help='Jitter sigma (ms)')
	p.add_argument('--drop-p', type=float, default=0.05, help='Frame drop probability')
	p.add_argument('--tracker-cmd', help='Optional tracker command to run after generation. Use {input} placeholder')
	p.add_argument('--seed', type=int, help='Random seed for reproducibility')
	args = p.parse_args()

	if args.seed is not None:
		random.seed(args.seed)

	if not os.path.exists(args.input):
		print('Input file not found:', args.input, file=sys.stderr)
		sys.exit(2)

	items = list(read_jsonl(args.input))

	if args.mode == 'lag':
		impaired = apply_fixed_lag(items, args.lag_ms)
	else:
		impaired = apply_jitter_and_drop(items, args.sigma_ms, args.drop_p)

	# Sort by new timestamp to simulate arrival order
	impaired.sort(key=lambda x: x.get('ts_ms', 0))
	write_jsonl(args.output, impaired)

	print(f'Wrote impaired replay to {args.output} (mode={args.mode})')

	if args.tracker_cmd:
		cmd = args.tracker_cmd.format(input=args.output)
		print('Invoking tracker:', cmd)
		try:
			ret = subprocess.run(cmd, shell=True)
		except Exception as e:
			print('Failed to start tracker command:', e, file=sys.stderr)
			sys.exit(3)
		if ret.returncode != 0:
			print('Tracker exited with non-zero code:', ret.returncode, file=sys.stderr)
			sys.exit(ret.returncode)


if __name__ == '__main__':
	main()

