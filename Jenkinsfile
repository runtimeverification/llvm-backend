pipeline {
  agent {
    dockerfile {
      additionalBuildArgs '--build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g)'
    }
  }
  options {
    ansiColor('xterm')
  }
  stages {
    stage("Init title") {
      when { changeRequest() }
      steps {
        script {
          currentBuild.displayName = "PR ${env.CHANGE_ID}: ${env.CHANGE_TITLE}"
        }
      }
    }
    stage('Build and Test') {
      options { timeout(time: 15, unit: 'MINUTES') }
      steps {
        sh '''
          ./ciscript Debug
          ./ciscript Release
          ./ciscript RelWithDebInfo
        '''
      }
    }
    stage('Update K Submodule') {
      when { branch 'master' }
      steps {
        build job: 'rv-devops/master', propagate: false, wait: false                                                      \
            , parameters: [ booleanParam(name: 'UPDATE_DEPS_SUBMODULE', value: true)                                      \
                          , string(name: 'PR_REVIEWER', value: 'dwightguth')                                              \
                          , string(name: 'UPDATE_DEPS_REPOSITORY', value: 'kframework/k')                                 \
                          , string(name: 'UPDATE_DEPS_SUBMODULE_DIR', value: 'llvm-backend/src/main/native/llvm-backend') \
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
