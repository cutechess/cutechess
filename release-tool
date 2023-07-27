#!/usr/bin/env python3

# tool to help make consistent Cute Chess releases

import os
import re
import subprocess
import sys
import tempfile


def usage() -> None:
    print("usage: release-tool <version>")
    sys.exit(1)


def error(msg: str) -> None:
    print("error:", msg, file=sys.stderr)
    sys.exit(1)


def validate_version(ver: str) -> bool:
    return re.match("^(0|[1-9]\\d*)\\.(0|[1-9]\\d*)\\.(0|[1-9]\\d*)", ver) is not None


def git_dir_exists() -> bool:
    return os.path.isdir(f"{os.getcwd()}/.git")


def current_branch() -> str:
    return (
        subprocess.run(["git", "branch", "--show-current"], stdout=subprocess.PIPE)
        .stdout.decode()
        .strip()
    )


def release_exists(rel: str) -> bool:
    return (
        len(
            subprocess.run(["git", "tag", "-l", f"v{rel}"], stdout=subprocess.PIPE)
            .stdout.decode()
            .strip()
        )
        != 0
    )


def working_tree_clean() -> bool:
    return (
        subprocess.run(["git", "diff-index", "--quiet", "HEAD", "--"]).returncode == 0
    )


def pull_remote_changes() -> bool:
    return subprocess.run(["git", "pull", "--rebase", "--ff-only"]).returncode == 0


def update_version_file(new_version: str) -> bool:
    try:
        with open(".version", "w") as f:
            f.write(new_version)
    except (OSError, IOError):
        return False

    # before entering to this function, working tree is clean
    # and ".version" should be already in the repo so
    # "commit -a" should be safe
    return (
        subprocess.run(
            ["git", "commit", "-a", "-m", f"version {new_version}"]
        ).returncode
        == 0
    )


if __name__ == "__main__":
    if len(sys.argv) != 2:
        usage()

    new_version = sys.argv[1]

    if not validate_version(new_version):
        error(
            f"{new_version} does not conform to the semver standard MAJOR.MINOR.PATCH"
        )

    if not git_dir_exists():
        error("'.git' does not exist; move to the source root")

    if current_branch() != "master":
        error("you're not in the 'master' branch")

    if not working_tree_clean():
        error("your working tree is not clean; commit or unstage changes")

    if release_exists(new_version):
        error(f"{new_version} already exists")

    if not pull_remote_changes():
        error("unable to pull remote changes")

    if not update_version_file(new_version):
        error("unable to update '.version' file")

    with tempfile.NamedTemporaryFile() as f:
        basename = os.path.basename(os.getcwd())
        f.write(f"{basename} {new_version}".encode())
        f.flush()
        subprocess.run(["git", "tag", "-e", "-F", f.name, "-a", f"v{new_version}"])
        print(new_version)
