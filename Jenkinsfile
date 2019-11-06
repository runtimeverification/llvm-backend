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
      options { timeout(time: 10, unit: 'MINUTES') }
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
        build job: 'rv-devops/master', parameters: [string(name: 'PR_REVIEWER', value: 'dwightguth'), booleanParam(name: 'UPDATE_DEPS_K_LLVM', value: true)], propagate: false, wait: false
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
