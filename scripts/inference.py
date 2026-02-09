"""Inference runner.

This script is a small wrapper to call a detection backend (Python-based YOLO CLI
or a TensorRT engine runner). It invokes a subprocess, forwards stdout/stderr,
and writes a small exit-status summary.
"""

import argparse
import subprocess
import sys
import shlex


def run_command(cmd, capture_output=False):
	print("Running:", cmd)
	args = shlex.split(cmd)
	try:
		res = subprocess.run(args, check=False, capture_output=capture_output, text=True)
	except FileNotFoundError:
		print("Command not found:", args[0], file=sys.stderr)
		return 127, "", ""
	if capture_output:
		return res.returncode, res.stdout, res.stderr
	return res.returncode, None, None


def main():
	p = argparse.ArgumentParser()
	p.add_argument("--cmd", help="Full command to run for inference (preferred)")
	p.add_argument("--engine", help="TensorRT engine path (optional)")
	p.add_argument("--source", help="Input source (video or images)")
	p.add_argument("--imgsz", default="640", help="Image size")
	p.add_argument("--capture-output", action="store_true", help="Capture stdout/stderr")
	args = p.parse_args()

	if args.cmd:
		cmd = args.cmd
	else:
		# fallback: example invocation for a yolovX CLI or custom trt runner
		if args.engine:
			cmd = f"yolo detect model={args.engine} source={args.source or '0'} imgsz={args.imgsz}"
		else:
			cmd = f"yolo detect model=weights/best.pt source={args.source or '0'} imgsz={args.imgsz}"

	code, out, err = run_command(cmd, capture_output=args.capture_output)
	if code != 0:
		print(f"Inference command exited with code {code}", file=sys.stderr)
		if args.capture_output:
			print("--- stdout ---")
			print(out)
			print("--- stderr ---")
			print(err, file=sys.stderr)
		sys.exit(code)

	print("Inference completed successfully (exit code 0)")


if __name__ == '__main__':
	main()

