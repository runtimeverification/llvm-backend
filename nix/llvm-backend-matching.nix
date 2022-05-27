{ buildMaven, src }:

let self = buildMaven {
  inherit src;

  doCheck = false;
  infoFile = ./llvm-backend-matching.mavenix.lock;

  passthru = {
    jar = "${self}/share/java/llvm-backend-matching-1.0-SNAPSHOT-jar-with-dependencies.jar";
  };

  postInstall = ''
    test -f "$out/share/java/llvm-backend-matching-1.0-SNAPSHOT-jar-with-dependencies.jar"
  '';

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
  # automatically. Update the mavenix lock file after changing `deps`.
  #
  deps = [
    {
      path = "org/scala-lang/scala-compiler/2.12.4/scala-compiler-2.12.4.jar";
      sha1 = "s0xw0c4d71qh8jgy1jipy385jzihx766";
    }
    {
      path = "org/scala-lang/scala-compiler/2.12.4/scala-compiler-2.12.4.pom";
      sha1 = "nx34986x5284ggylf3bg8yd36hilsn5i";
    }
  ];

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
