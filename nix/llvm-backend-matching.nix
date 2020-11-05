{ mavenix }:

let self = mavenix.buildMaven {
  src = ../matching;
  doCheck = false;
  infoFile = ./llvm-backend-matching.mavenix.lock;

  passthru = {
    jar = "${self}/share/java/llvm-backend-matching-1.0-SNAPSHOT-jar-with-dependencies.jar";
  };

  # Add build dependencies
  #
  #buildInputs = with pkgs; [ git makeWrapper ];

  # Set build environment variables
  #
  #MAVEN_OPTS = "-Dfile.encoding=UTF-8";

  # Attributes are passed to the underlying `stdenv.mkDerivation`, so build
  #   hooks can be set here also.
  #
  #postInstall = ''
  #  makeWrapper ${pkgs.jre8_headless}/bin/java $out/bin/my-bin \
  #    --add-flags "-jar $out/share/java/my-proj.jar"
  #'';

  # Add extra maven dependencies which might not have been picked up
  #   automatically
  #
  #deps = [
  #  { path = "org/group-id/artifactId/version/file.jar"; sha1 = "0123456789abcdef"; }
  #  { path = "org/group-id/artifactId/version/file.pom"; sha1 = "123456789abcdef0"; }
  #];

  # Add dependencies on other mavenix derivations
  #
  #drvs = [ (import ../other/mavenix/derivation {}) ];

  # Override which maven package to build with
  #
  #maven = maven.overrideAttrs (_: { jdk = pkgs.oraclejdk10; });

  # Override remote repository URLs and settings.xml
  #
  #remotes = { central = "https://repo.maven.apache.org/maven2"; };
  #settings = ./settings.xml;
}; in self
