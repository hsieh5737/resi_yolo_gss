#!/bin/bash
yolo export model=weights/best.pt format=engine device=0 half=True
