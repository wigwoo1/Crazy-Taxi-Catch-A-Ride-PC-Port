#!/usr/bin/env python3
"""Verify allowlisted text-free texture blending and fail-closed rejection."""
import copy
import hashlib
import importlib.util
import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
ASSET = ROOT / "assets/texture-ai-experiments/city1-tile-141-text-free-approved"
BASE = ASSET / "source-bicubic-4x-128.png"
DETAIL = ASSET / "ai-detail-text-free-128.png"
APPROVAL = ASSET / "text-free-approval.json"
BLENDER = ROOT / "tools/blend_ai_texture_detail.py"
SPEC = importlib.util.spec_from_file_location("texture_blender", BLENDER)
BLENDER_MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(BLENDER_MODULE)


def main():
    approval = json.loads(APPROVAL.read_text(encoding="utf-8"))
    good_output = ASSET / "source-preserving-4x-rebuilt.png"
    good_report = ROOT / "reports/texture-ai-trial/texture141-source-preserving/text-free-rebuild.json"
    bad_manifest = ASSET / "text-exclusion-negative-approval.tmp.json"
    bad_output = ASSET / "must-not-exist-text-exclusion-test.png"
    bad_report = ASSET / "must-not-exist-text-exclusion-test.json"
    if any(path.exists() for path in (bad_manifest, bad_output, bad_report)):
        raise FileExistsError("refusing to overwrite a pre-existing text-exclusion test artifact")
    try:
        sys.argv = [str(BLENDER), str(BASE), str(DETAIL), str(good_output),
                    str(good_report), "--radius", "12", "--strength", "0.20",
                    "--edge-fade", "6", "--text-free-approval", str(APPROVAL)]
        BLENDER_MODULE.main()
        report = json.loads(good_report.read_text(encoding="utf-8"))
        actual = hashlib.sha256(good_output.read_bytes()).hexdigest()
        assert actual == report["output_sha256"]
        assert report["text_exclusion_enforced"] is True
        assert set(report["excluded_content"]) == {
            "text", "fonts", "logos", "signage", "lettering"
        }

        bad_approval = copy.deepcopy(approval)
        bad_approval["input_sha256"]["generated_detail"] = "0" * 64
        bad_manifest.write_text(json.dumps(bad_approval), encoding="utf-8")
        sys.argv = [str(BLENDER), str(BASE), str(DETAIL), str(bad_output),
                    str(bad_report), "--radius", "12", "--strength", "0.20",
                    "--edge-fade", "6", "--text-free-approval", str(bad_manifest)]
        try:
            BLENDER_MODULE.main()
        except SystemExit:
            pass
        else:
            raise AssertionError("unapproved generated-detail input was accepted")
        assert not bad_output.exists(), "rejected input still produced an upscale"
    finally:
        bad_manifest.unlink(missing_ok=True)
        bad_output.unlink(missing_ok=True)
        bad_report.unlink(missing_ok=True)
    print("PASS: approved text-free upscale rebuilds; altered image hash is rejected before output")


if __name__ == "__main__":
    main()
