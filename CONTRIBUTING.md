# Contributing

When contributing to this repository, please first discuss the change you
wish to make via an issue, email to the maintainers, or any other method
with the maintainers of this repository before making a change.

Please note we have a code of conduct, please follow it in all your
interactions with the project.

## Pull Request Process

1.  Update CHANGELOG.md with details of the changes implemented. This includees
    any tests created as well
2.  Don't break any old tests or functionality when updating unless the test has
    been determined to be wrong (requires discussion beforehand)
3.  Add tests for any new functionality in the appropriate `test` folder 
    in the source tree. Tests should be comprehensive to make sure the kernel
    does not lose stability.
4.  Any commits must be gpg signed with a key associated with your account.
    This keeps a chain of custody to allow proper back tracking of commits.
5.  Pull requests should be flattened into a single commit. 

## Coding style
All code should follow the standard coding style enforced herein.

1.  80 column lines. This should be practiced as a hard rule. If there are are 
    commands that break this, consider rewriting them, or splitting them between
    two lines.
2.  K&R style bracing should be used consistently.
3.  Comments should be used, but not overused for obvious content. Comments
    should use the c++ `\\` style except for file descriptors at the beginning
    of each file. Comments should not include any curses or offensive content.
4.  Avoid monolithic functions. Indented code should never drop below 4 indents.
    Consider exposing that to a seperate function or rewriting the logic at that
    point.

## Code of Conduct

### Our Pledge

In the interest of fostering an open and welcoming environment, we as
contributors and maintainers pledge to make participation in our project and
community a harrasment-free zone for everyone, regardless of age, disability,
ethnicity, gender identity and expression, level of experience, nationality,
appearance, race, religion, or sexual identiy and orientation. Please note that
this is not an exhaustive list. Any harrasment of any kind will not be allowed
or tolerated.

### Our Standards
Examples of behavior that contributes to creating a positive environment
include:

* Using welcoming and inclusive language
* Being respectful of differing viewpoints and experiences
* Gracefully accepting constructive criticism
* Focusing on what is best for the community
* Showing empathy towards other community members

Examples of unacceptable behavior by participants include:

* The use of sexualized language or imagery and unwelcome sexual attention or
advances
* Trolling, insulting/derogatory comments, and personal or political attacks
* Public or private harassment
* Publishing others' private information, such as a physical or electronic
  address, without explicit permission
* Other conduct which could reasonably be considered inappropriate in a
  professional setting

### Our Responsibilities

Project maintainers are responsible for clarifying the standards of acceptable
behavior and are expected to take appropriate and fair corrective action in
response to any instances of unacceptable behavior.

Project maintainers have the right and responsibility to remove, edit, or
reject comments, commits, code, wiki edits, issues, and other contributions
that are not aligned to this Code of Conduct, or to ban temporarily or
permanently any contributor for other behaviors that they deem inappropriate,
threatening, offensive, or harmful.

### Scope

This Code of Conduct applies both within project spaces and in public spaces
when an individual is representing the project or its community. Examples of
representing a project or community include using an official project e-mail
address, posting via an official social media account, or acting as an appointed
representative at an online or offline event. Representation of a project may be
further defined and clarified by project maintainers.

### Enforcement

Instances of abusive, harassing, or otherwise unacceptable behavior may be
reported by contacting the project team at [INSERT EMAIL ADDRESS]. All
complaints will be reviewed and investigated and will result in a response that
is deemed necessary and appropriate to the circumstances. The project team is
obligated to maintain confidentiality with regard to the reporter of an incident.
Further details of specific enforcement policies may be posted separately.

Project maintainers who do not follow or enforce the Code of Conduct in good
faith may face temporary or permanent repercussions as determined by other
members of the project's leadership.

### Attribution

This Code of Conduct is adapted from the [Contributor Covenant][homepage], version 1.4,
available at [http://contributor-covenant.org/version/1/4][version]

[homepage]: http://contributor-covenant.org
[version]: http://contributor-covenant.org/version/1/4/
