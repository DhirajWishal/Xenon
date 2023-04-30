# Know your branches

This repository uses a bunch of branches, which contain some information about what's going on. The main branch is `release` which contains the stable code which will most probably work on all the supported platforms without an issue. Then we have a bunch more branches and folders and they go like this,

* `bugfix/`: This is a folder and may contain a bunch of bug-fixing branches, dedicated to a single bug.
* `hotfix/`: This is a folder and may contain a bunch of hot-fixing branches.
* `feature/`: This is a folder and may contain a bunch of feature implementations.
* `backup/`: This is a folder and will contain multiple branches with different backups of the repository. This is used if we're removing something important from the repository and if we might need it in the future.
* `platform/`: This is a folder and contains platform-specific implementations. This folder is mainly used when adding support for a new platform.
* `quickfix/`: This is a folder that contains quick fixes that don't need a formal issue or anything else. For dumb issues that we make as hoomans...
* `experimental/*`: This is a folder that contains all the experimental features that might be implemented. This is used to test a new system before either implementing it or merging to a feature before merging to the `dev` branch. If anything new is added to the engine using one of these branches, they will be added inside the `Xenon::Experimental` namespace.
* `cleanup`: This is a single branch and is used when doing maintenance things on the codebase.
* `dev`: This is the main development branch. This is where we actively develop things before finally merging into `release`.

## How to create a branch?

There are a few rules to follow when creating a branch.

1. New branches always require a folder.
2. If a branch is created for a single issue, the branch name should be `<folder>/issue-<issue number>` where the `<folder>` is the appropriate folder the issue's fix or improvement falls into, and `<issue number>` is the issue's number (denoted as `#n` in the issue).
3. If a branch is created as a new feature, or an issue is not created for it, make sure that it still has a folder and the branch name should be meaningful.
4. Re-use branches whenever necessary.
5. Don't you dare create new folders! Please use the available folders to create new branches.

*Note that this goes out to contributors of the repository!*
