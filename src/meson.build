globber = run_command('sh', '-c', 'find . -name "*.cpp" | sort', check: true)
src = globber.stdout().strip().split('\n')

executable('Voxelaria', src,
  dependencies : [
    dependency('glew'),
    dependency('glm'),
    dependency('glfw3')
  ],
  install : true)
