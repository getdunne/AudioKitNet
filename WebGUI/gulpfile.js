'use strict';

// general
var gulp = require('gulp');
var del = require('del');
var runSequence = require('run-sequence');
var newer = require('gulp-newer');

// sass
var sass = require('gulp-sass');
var autoprefixer = require('gulp-autoprefixer');

// javascript
var uglify = require('gulp-uglify');
var concat = require('gulp-concat');

// sourcemaps
var sourcemaps = require('gulp-sourcemaps');

// flags
var env = 'dev'; // default
var gulpif = require('gulp-if');

/**
 * Variables
 */

var sourceDir = 'src';
var destDir = 'build';

var config = {
  cssSrcDir: sourceDir + '/scss',
  cssDestDir: destDir + '/',
  jsSrcDir: sourceDir + '/js',
  jsDestDir: destDir + '/',
  viewSrcDir: sourceDir + '/views',
  viewDestDir: destDir + '/',
  autoprefixer: {
    browsers: ['> 1%', 'last 2 versions'],
    cascade: true,
    remove: true
  }
};

/**
 * Primary Tasks
 */

gulp.task('build', function(cb) {
  runSequence('clean', ['styles', 'scripts', 'views'], cb);
});

gulp.task('watch', ['build'], function() {
  gulp.watch(config.cssSrcDir + '/**/*.*', ['styles']);
  gulp.watch(config.jsSrcDir + '/**/*.*', ['scripts']);
  gulp.watch(config.viewSrcDir + '/**/*.*', ['views']);
});

gulp.task('deploy', function() {
  env = 'prod';
  gulp.start('build');
});

gulp.task('default', function() {
  console.log('');
  console.log('To do a basic build, run \"gulp build\"');
  console.log('');
  console.log('To build and rebuild on changes, run \"gulp watch\"');
  console.log('');
  console.log('To build for production, run \"gulp deploy\"');
  console.log('');
});

/**
 * Task Details
 */

gulp.task('clean', function() {
  return del([destDir]);
});

gulp.task('styles', function() {
  var files = config.cssSrcDir + '/**/*.scss';

  var sassOptions = {
    outputStyle: (env === 'dev') ? 'nested' : 'compressed'
  };

  return gulp.src(files)
    .pipe(gulpif(env === 'dev', sourcemaps.init()))
    .pipe(sass(sassOptions).on('error', sass.logError))
    .pipe(autoprefixer(config.autoprefixer))
    .pipe(gulpif(env === 'dev', sourcemaps.write('maps')))
    .pipe(gulp.dest(config.cssDestDir));
});

gulp.task('scripts', function() {
  var files = [
    config.jsSrcDir + '/**/*.js'
  ];

  var uglifyOptions = {
    compress: {
      'drop_console': true
    }
  };

  var outputFilename = 'main.js';

  return gulp.src(files)
    .pipe(newer(config.jsDestDir + '/' + outputFilename))
    .pipe(gulpif(env === 'dev', sourcemaps.init()))
    .pipe(concat(outputFilename))
    .pipe(gulpif(env === 'dev', sourcemaps.write('maps')))
    .pipe(gulpif(env === 'prod', uglify(uglifyOptions)))
    .pipe(gulp.dest(config.jsDestDir));
});

gulp.task('views', function() {
  var files = [
    config.viewSrcDir + '/**/*.html'
  ];

  return gulp.src(files)
    .pipe(gulp.dest(config.viewDestDir));
});
