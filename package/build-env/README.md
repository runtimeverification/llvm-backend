# The build environment

`package/debian/` holds the *recipe* for the Debian package. This directory holds the
*environment* it is built in, and the entry points that drive it.

```
package/build-env/Dockerfile    the build environment (clang, cmake, maven, JDK, lit)
package/build-env/build-deb     build the .deb for one Ubuntu release
package/build-env/test-deb      install that .deb into a clean image, as root
package/build-env/common.sh     the distro table both scripts read
```

```
package/build-env/build-deb noble
package/build-env/test-deb  noble
```

Both take an optional second argument naming the `.deb`; the default is
`k-llvm-backend_amd64_ubuntu_<distro>.deb`, left at the root of the checkout. They can be run
from any directory — they `cd` to the checkout root themselves, because
`package/debian/build-package` assumes it. They use `podman` if it is installed and `docker`
otherwise; `CONTAINER_ENGINE` overrides that.

Which LLVM version goes with which Ubuntu release lives in `common.sh` and nowhere else, so
adding a release is one edit here plus one in the CI matrix.

`build-package` *moves* files out of `package/debian/` into a top-level `debian/`, so a second
`build-deb` run needs a clean tree. That is pre-existing behaviour, noted here because it is
surprising the first time.

## Why it lives here

It used to be `.github/workflows/Dockerfile`, invoked by two composite actions under
`.github/actions/`. That made the environment reachable only from a GitHub workflow, and the
knowledge of *how to invoke it* — which LLVM version pairs with which Ubuntu release, which
build args the Dockerfile needs, how to map the calling user into the container, where to
mount the checkout — was not written down as anything executable. It was spread across two
action definitions and then copied, imperfectly, into whatever else needed to build a deb.

Moving CI to the internal forge deletes `.github/` entirely. Rather than move the same
arrangement sideways, the environment moved next to the recipe it builds, behind two scripts
that a forge workflow, the release repository, and a developer's shell can all call the same
way.

## Two things that are easy to get wrong

**The checkout mounts at `/opt/workspace/source`, not at `/opt/workspace`.**
`dpkg-buildpackage` writes the `.deb` to the *parent* of the directory it builds in, and
`package/debian/build-package` then moves it back. Mount the checkout at `/opt/workspace` and
that parent is `/`, which the non-root build user cannot write — the build then fails from
inside `dpkg-buildpackage`, naming neither the mount nor the reason. The `Dockerfile` creates
`/opt/workspace` owned by the build user so that the parent is writable.

**The package must be tested somewhere other than the build environment.** That image has
every build dependency already installed, so a missing or wrong entry in `debian/control`
would install cleanly there and fail only for a user. `test-deb` uses a bare
`ubuntu:<distro>` for that reason, and needs root, which the build environment does not have.

## The account inside the image

The build runs as an account whose uid/gid match the caller's, so files written to the
bind-mounted checkout come back owned by whoever ran the build. Only the *ids* matter for
that, so the `Dockerfile` names the account itself (`builder`) and callers pass ids alone.

Passing the caller's account *name* through — which is what this used to do — collides with
whatever the base image already ships. `ubuntu:noble` has an `ubuntu` account at uid/gid 1000,
and `users` (gid 100) is the primary group of many developer accounts; either one makes
`useradd` fail several minutes into the build, with a message that never mentions a build arg.
`-o` lets the ids be shared rather than deleting a system account that something else uses.
