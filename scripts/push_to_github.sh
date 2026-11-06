#!/usr/bin/env bash
# Kich ban dong bo toan bo du an len kho luu tru GitHub kuroquang2302-lab/Balancing-Bike
set -e

if [ ! -d ".git" ]; then
    echo "Khoi tao kho luu tru Git..."
    git init
    git remote add origin https://github.com/kuroquang2302-lab/Balancing-Bike.git
fi

git branch -M main
git add .
git commit -m "feat: update four-layer hybrid controller, verified references, and mass budget"
git push -u origin main

echo "Dong bo len GitHub hoan tat."
