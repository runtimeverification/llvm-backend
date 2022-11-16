pipeline {
  agent none
  options {
    ansiColor('xterm')
  }
  stages {
    stage('Update K Submodule') {
      when { branch 'master' }
      agent {
        dockerfile {
          additionalBuildArgs '--build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g)'
          reuseNode true
        }
      }
      environment { LONG_REV = """${sh(returnStdout: true, script: 'git rev-parse HEAD').trim()}""" }
      steps {
        build job: 'DevOps/master', propagate: false, wait: false                                                    \
            , parameters: [ booleanParam ( name: 'UPDATE_DEPS'         , value: true                               ) \
                          , string       ( name: 'UPDATE_DEPS_REPO'    , value: 'runtimeverification/llvm-backend' ) \
                          , string       ( name: 'UPDATE_DEPS_VERSION' , value: "${env.LONG_REV}"                  ) \
                          ]
      }
    }
  }
  post {
    unsuccessful {
      script {
        if (env.BRANCH_NAME == 'master') {
          slackSend color: '#cb2431'                                    \
                    , channel: '#llvm-backend'                          \
                    , message: "Master build failure: ${env.BUILD_URL}"
        }
      }
    }
  }
}
