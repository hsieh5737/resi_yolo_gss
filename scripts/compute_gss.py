import cv2
import csv
import os
import sys

V_THR = 217
S_THR = 38
ROI_FRAC = 0.65

def glare_level(gss):
    if gss < 0.3:
        return "Low"
    elif gss < 0.6:
        return "Med"
    else:
        return "High"

img_dir = sys.argv[1]
out_csv = sys.argv[2]

with open(out_csv, "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["filename", "gss_value", "glare_level"])

    for fn in sorted(os.listdir(img_dir)):
        if not fn.lower().endswith((".jpg", ".png")):
            continue

        img = cv2.imread(os.path.join(img_dir, fn))
        if img is None:
            continue

        h = img.shape[0]
        roi = img[:int(h * ROI_FRAC), :]

        hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
        mask = (hsv[...,2] >= V_THR) & (hsv[...,1] <= S_THR)

        gss = mask.sum() / mask.size
        writer.writerow([fn, f"{gss:.4f}", glare_level(gss)])
