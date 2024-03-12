{ maven, src, mvnHash }:

let self = maven.buildMavenPackage rec {
    inherit src mvnHash;
    pname = "llvm-backend-matching";
    version = "0";

    buildOffline = true;

    manualMvnArtifacts = [
      "org.scala-lang:scala-compiler:2.12.18"
      "org.apache.maven.plugins:maven-compiler-plugin:3.7.0"
    ];

    passthru = {
      jar =
        "${self}/share/java/llvm-backend-matching-1.0-SNAPSHOT-jar-with-dependencies.jar";
    };

    installPhase = ''
      mkdir -p $out/share/java
      install -Dm644 target/*.jar $out/share/java
      test -f $out/share/java/llvm-backend-matching-1.0-SNAPSHOT-jar-with-dependencies.jar
    '';
  };
in self
