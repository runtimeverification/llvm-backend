pipeline {
  agent none
  options {
    ansiColor('xterm')
  }
  stages {
    stage("Init title") {
      when { changeRequest() }
      steps { script { currentBuild.displayName = "PR ${env.CHANGE_ID}: ${env.CHANGE_TITLE}" } }
    }
    stage('Build and Test on Arch Linux') {
      options { timeout(time: 25, unit: 'MINUTES') }
      agent {
        dockerfile {
          filename 'Dockerfile.arch'
          additionalBuildArgs '--build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) --pull'
        }
      }
      steps {
        sh '''
          #./ciscript Debug
          #./ciscript Release
          #./ciscript RelWithDebInfo
          #./ciscript FastBuild
          #./ciscript GcStats

        '''
      }
    }
    stage('Build and Test on Ubuntu') {
      options { timeout(time: 25, unit: 'MINUTES') }
      agent {
        dockerfile {
          additionalBuildArgs '--build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g)'
          reuseNode true
        }
      }
      steps {
        sh '''
          ./ciscript Debug
          ./ciscript Release
          ./ciscript RelWithDebInfo
          ./ciscript FastBuild
          ./ciscript GcStats
        '''
      }
    }
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
        build job: 'DevOps/master', propagate: false, wait: false                                           \
            , parameters: [ booleanParam ( name: 'UPDATE_DEPS'         , value: true                      ) \
                          , string       ( name: 'UPDATE_DEPS_REPO'    , value: 'kframework/llvm-backend' ) \
                          , string       ( name: 'UPDATE_DEPS_VERSION' , value: "${env.LONG_REV}"         ) \
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
