#!/usr/bin/env bash

set -euxo pipefail

SCALA_KORE_VERSION="$(cat matching/deps/scala_kore_release)"
changed=false

sed -i 's!^    <scala-kore.version>.*</scala-kore.version>$!    <scala-kore.version>'"${SCALA_KORE_VERSION}"'</scala-kore.version>!' matching/pom.xml
if git add matching/pom.xml && git commit -m "matching: update scala-kore to ${SCALA_KORE_VERSION}"; then
  changed=true
fi

if [ "${changed}" = "true" ]; then
  sed -i 's!    mvnHash = "sha256-.*";!    mvnHash = "";!' nix/overlay.nix
  (git add nix/overlay.nix && git commit -m "nix: invalidate maven packages hash") || true
fi
