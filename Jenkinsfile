pipeline {
  agent {
    dockerfile {
      args '-u 0'
    }
  }
  options {
    skipDefaultCheckout true
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
    stage('Checkout code') {
      steps {
        sh 'rm -rf ./*'
        checkout scm
      }
    }
    stage('Build and Test') {
      steps {
        ansiColor('xterm') {
          sh '''
            ./ciscript
          '''
        }
      }
    }
  }
}
