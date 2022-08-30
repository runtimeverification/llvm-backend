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
    stage('Build and test Arch Linux') {
      agent {
        dockerfile {
          filename 'Dockerfile.arch'
          additionalBuildArgs '--build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) --pull'
        }
      }
      options { timeout(time: 30, unit: 'MINUTES') }
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
    stage('Build and test Ubuntu Focal') {
      agent {
        dockerfile {
          additionalBuildArgs '--build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) --build-arg BASE_IMAGE=ubuntu:focal --build-arg LLVM_VERSION=10'
          reuseNode true
        }
      }
      options { timeout(time: 30, unit: 'MINUTES') }
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
    stage('Build and test Ubuntu Jammy') {
      agent {
        dockerfile {
          additionalBuildArgs '--build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) --build-arg BASE_IMAGE=ubuntu:jammy --build-arg LLVM_VERSION=14'
          reuseNode true
        }
      }
      options { timeout(time: 30, unit: 'MINUTES') }
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
